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

void cleanup(int sig)
{
    printf("\nExiting...\n");
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

    if (gpio_init(&button, CHIP_NUM, BUTTON_LINE, GPIO_INPUT) < 0)
    {
        printf("Button init failed\n");
        gpio_close(&led);
        return -1;
    }

    // ---------- TIMER ----------
    int tfd = timerfd_create(CLOCK_MONOTONIC, 0);
    if (tfd < 0)
    {
        perror("timerfd_create failed");
        return -1;
    }

    struct itimerspec ts = {
        .it_value = {1, 0},
        .it_interval = {1, 0}
    };

    timerfd_settime(tfd, 0, &ts, NULL);

    // ---------- POLL ----------
    struct pollfd fds[2];

    // button fd (get from internal struct)
    fds[0].fd = gpiod_line_event_get_fd(button.line);
    fds[0].events = POLLIN;

    // timer fd
    fds[1].fd = tfd;
    fds[1].events = POLLIN;

    // ---------- STATE ----------
    typedef enum {
        STATE_OFF,
        STATE_ON,
        STATE_BLINK
    } state_t;

    state_t state = STATE_OFF;
    int led_state = 0;

    struct timespec press_time;
    int pressed = 0;
    long long_press_ms = 1000;

    printf("System started\n");

    while (1)
    {
        poll(fds, 2, -1);

        // ---------- BUTTON ----------
        if (fds[0].revents & POLLIN)
        {
            int event = gpio_read_event(&button);
            if (event < 0)
                continue;

            struct timespec now;
            clock_gettime(CLOCK_MONOTONIC, &now);

            // PRESS
            if (event == GPIOD_LINE_EVENT_RISING_EDGE)
            {
                press_time = now;
                pressed = 1;
            }

            // RELEASE
            else if (event == GPIOD_LINE_EVENT_FALLING_EDGE)
            {
                if (!pressed)
                    continue;

                long diff = (now.tv_sec - press_time.tv_sec) * 1000 +
                            (now.tv_nsec - press_time.tv_nsec) / 1000000;

                pressed = 0;

                // ---------- SHORT PRESS ----------
                if (diff < long_press_ms)
                {
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

                    gpio_set(&led, led_state);
                }
                // ---------- LONG PRESS ----------
                else
                {
                    if (state == STATE_BLINK)
                    {
                        state = STATE_OFF;
                        led_state = 0;
                    }
                    else
                    {
                        state = STATE_BLINK;
                    }

                    gpio_set(&led, led_state);
                }
            }
        }

        // ---------- TIMER ----------
        if (fds[1].revents & POLLIN)
        {
            uint64_t exp;
            read(tfd, &exp, sizeof(exp));

            if (state == STATE_BLINK)
            {
                led_state = !led_state;
                gpio_set(&led, led_state);
            }
        }
    }

    return 0;
}



// int main()
// {
//     signal(SIGINT, cleanup);

//     if (gpio_init(&led, CHIP_NUM, LED_LINE, GPIO_OUTPUT) < 0)
//     {
//         fprintf(stderr, "LED init failed\n");
//         printf("LED init failed\n");
//         return -1;
//     }
//     printf("LED initialized on chip %d, line %d\n", CHIP_NUM, LED_LINE);

//     if (gpio_init(&button, CHIP_NUM, BUTTON_LINE, GPIO_INPUT) < 0)
//     {
//         fprintf(stderr, "Button init failed\n");
//         printf("Button init failed\n");
//         gpio_close(&led);
//         return -1;
//     }
//     printf("Button initialized on chip %d, line %d\n", CHIP_NUM, BUTTON_LINE);

//     struct timespec last = {0};
//     long debounce_ms = 200;

//     int led_state = 0;

//     printf("Press button to toggle LED (Ctrl+C to exit)\n");

//     while (1)
//     {
//         printf("Waiting for button press...\n");
//         int ret = gpio_wait_for_event(&button, -1);
//         printf("Event detected: %d\n", ret);

//         if (ret <= 0)
//             continue;

//         int event = gpio_read_event(&button);
//         if (event < 0)
//             continue;

//         struct timespec now;
//         clock_gettime(CLOCK_MONOTONIC, &now);

//         long diff = (now.tv_sec - last.tv_sec) * 1000 +
//                     (now.tv_nsec - last.tv_nsec) / 1000000;

//         if (diff < debounce_ms)
//             continue;

//         last = now;

//         if (event == GPIO_EVENT_RISING_EDGE)
//         {
//             led_state = !led_state;
//             gpio_set(&led, led_state);

//             printf("LED: %d\n", led_state);
//         }
//     }

//     return 0;
// }


// int main()
// {   
  
//     signal(SIGINT,cleanup);
//     int ret = gpio_init(&led,CHIP_NUM,LED_LINE,GPIO_OUTPUT);
//     if(ret<0)
//     {
//         printf("LED init failed\n");
//         return -1;
//     }   
//     printf("LED initialized on chip %d, line %d\n", CHIP_NUM, LED_LINE);    
    
//     int fd = timerfd_create(CLOCK_MONOTONIC,0);
//     if(fd<0)    {
//         perror("Failed to create timerfd");
//         gpio_close(&led);
//         return -1;
//     }
//     struct itimerspec ts={
//         .it_value.tv_sec=2,
//         .it_value.tv_nsec=0,
//         .it_interval.tv_sec=2,
//         .it_interval.tv_nsec=0              
//     }   ;
//     if(timerfd_settime(fd,0,&ts,NULL)<0)    {
//         perror("Failed to set timer");
//         close(fd);
//         gpio_close(&led);
//         return -1;
//     }
//     int led_status =0;

//     while(1)
//     {
//         uint64_t exp;
//         read(fd,&exp,sizeof(exp));
//         led_status = !led_status;
//         if(gpio_set(&led,led_status)<0)       
//          {
//              printf("Failed to set LED state\n");
//              break;
//          }  
//          printf("blink\n");
        
//     }

//     return 0;
// }





