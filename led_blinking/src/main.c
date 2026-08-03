#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <time.h>
#include <signal.h>
#include <poll.h>
#include <sys/timerfd.h>
#include "gpio.h"

#define CHIP_NUM 1
#define LED_LINE 21
#define BUTTON_LINE 7

static gpiod_t led, button;

#define DBG(fmt, ...) printf("DEBUG: " fmt "\n", ##__VA_ARGS__)

static const char *state_to_string(int state)
{
    switch (state)
    {
        case 0: return "OFF";
        case 1: return "ON";
        case 2: return "BLINK";
        default: return "UNKNOWN";
    }
}
// ---------- STATE ----------
    typedef enum {
        STATE_OFF,
        STATE_ON,
        STATE_BLINK
    } state_t;


void cleanup(int sig)
{
    printf("\nExiting...\n");
    DBG("Cleanup called (signal=%d)", sig);
    gpio_close(&led);
    gpio_close(&button);
    _exit(0);
}

int main()
{
    signal(SIGINT, cleanup);

    // ---------- INIT ----------
    if (gpio_init(&led, CHIP_NUM, LED_LINE, GPIO_OUTPUT) < 0)
    {
        printf("LED init failed\n");
        return -1;
    }
    DBG("LED initialized on chip %d, line %d", CHIP_NUM, LED_LINE);

    if (gpio_init(&button, CHIP_NUM, BUTTON_LINE, GPIO_INPUT) < 0)
    {
        printf("Button init failed\n");
        gpio_close(&led);
        return -1;
    }
    DBG("Button initialized on chip %d, line %d", CHIP_NUM, BUTTON_LINE);

    // ---------- TIMER ----------
    int tfd = timerfd_create(CLOCK_MONOTONIC, 0);
    if (tfd < 0)
    {
        perror("timerfd_create failed");
        return -1;
    }
    DBG("Timerfd created, fd=%d", tfd);

    struct itimerspec ts = {
        .it_value = {1, 0},
        .it_interval = {1, 0}
    };

    if (timerfd_settime(tfd, 0, &ts, NULL) < 0)
    {
        perror("timerfd_settime failed");
        return -1;
    }
    DBG("Timer configured for 1s intervals");

    // ---------- POLL ----------
    struct pollfd fds[2];

    // button fd (get from internal struct)
    fds[0].fd = gpiod_line_event_get_fd(button.line);
    fds[0].events = POLLIN;

    // timer fd
    fds[1].fd = tfd;
    fds[1].events = POLLIN;

    state_t state = STATE_OFF;
    int led_state = 0;

    struct timespec press_time;
    struct timespec last_button_event = {0};
    int pressed = 0;
    long long_press_ms = 1000;
    long debounce_ms = 50;

    printf("System started\n");
    DBG("Starting main loop, initial state=%s", state_to_string(state));

    while (1)
    {
        int ret = poll(fds, 2, -1);
        if (ret < 0)
        {
            perror("poll failed");
            continue;
        }

        // ---------- BUTTON ----------
        if (fds[0].revents & POLLIN)
        {
            int event = gpio_read_event(&button);
            if (event < 0)
                continue;

            struct timespec now;
            clock_gettime(CLOCK_MONOTONIC, &now);

            long debounce_diff = (now.tv_sec - last_button_event.tv_sec) * 1000 +
                                 (now.tv_nsec - last_button_event.tv_nsec) / 1000000;
            if (last_button_event.tv_sec != 0 && debounce_diff < debounce_ms)
            {
                DBG("Button event ignored by debounce (%ld ms)", debounce_diff);
                continue;
            }
            last_button_event = now;

            if (event == GPIOD_LINE_EVENT_RISING_EDGE)
            {
                press_time = now;
                pressed = 1;
                DBG("Button pressed");
            }
            else if (event == GPIOD_LINE_EVENT_FALLING_EDGE)
            {
                if (!pressed)
                    continue;

                long diff = (now.tv_sec - press_time.tv_sec) * 1000 +
                            (now.tv_nsec - press_time.tv_nsec) / 1000000;

                pressed = 0;
                DBG("Button released after %ld ms", diff);

                int old_state = state;

                if (diff < long_press_ms)
                {
                    DBG("Short press detected");
                    if (state == STATE_OFF)
                    {
                        state = STATE_ON;
                        led_state = 1;
                    }
                    else if (state == STATE_ON)
                    {
                        state = STATE_OFF;
                        led_state = 0;
                    }
                    else if (state == STATE_BLINK)
                    {
                        state = STATE_OFF;
                        led_state = 0;
                    }
                }
                else
                {
                    DBG("Long press detected");
                    if (state == STATE_BLINK)
                    {
                        state = STATE_OFF;
                        led_state = 0;
                    }
                    else
                    {
                        state = STATE_BLINK;
                    }
                }

                if (old_state != state)
                {
                    DBG("State changed %s -> %s", state_to_string(old_state), state_to_string(state));
                }
                else
                {
                    DBG("State remains %s", state_to_string(state));
                }

                if (gpio_set(&led, led_state) < 0)
                {
                    DBG("Failed to update LED state to %d", led_state);
                }
                else
                {
                    DBG("LED set to %d", led_state);
                }
            }
        }

        // ---------- TIMER ----------
        if (fds[1].revents & POLLIN)
        {
            uint64_t exp;
            if (read(tfd, &exp, sizeof(exp)) < 0)
            {
                perror("Failed to read timerfd");
                continue;
            }
            DBG("Timer event expired %llu times", (unsigned long long)exp);

            if (state == STATE_BLINK)
            {
                led_state = !led_state;
                DBG("Blink mode toggling LED to %d", led_state);
                gpio_set(&led, led_state);
            }
        }
    }

    return 0;
}

