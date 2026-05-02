#include <stdio.h>
#include <string.h>
#include <time.h>
#include "gpio.h"

int gpio_init(gpiod_t *gpio, int chip_num, int line_num, gpio_direction direction)
{
    char chip_name[20];

    snprintf(chip_name, sizeof(chip_name), "gpiochip%d", chip_num);

    gpio->chip = gpiod_chip_open_by_name(chip_name);
    if (!gpio->chip) {
        perror("Failed to open GPIO chip");
        return -1;
    }

    gpio->line = gpiod_chip_get_line(gpio->chip, line_num);
    if (!gpio->line) {
        perror("Failed to get GPIO line");
        gpiod_chip_close(gpio->chip);
        return -1;
    }

    if (direction == GPIO_INPUT)
    {
        if (gpiod_line_request_rising_edge_events(gpio->line, "gpio_input") < 0)
        {
            perror("Failed to request event line");
            gpiod_line_release(gpio->line);
            gpiod_chip_close(gpio->chip);
            return -1;
        }
    }
    else if (direction == GPIO_OUTPUT)
    {
        if (gpiod_line_request_output(gpio->line, "gpio_output", 0) < 0)
        {
            perror("Failed to request output line");
            gpiod_line_release(gpio->line);
            gpiod_chip_close(gpio->chip);
            return -1;
        }
    }
    else
    {
        fprintf(stderr, "Invalid GPIO direction\n");
        gpiod_chip_close(gpio->chip);
        return -1;
    }

    return 0;
}

int gpio_set(gpiod_t *gpio, int val)
{
    if (gpiod_line_set_value(gpio->line, val) < 0)
    {
        perror("Failed to set GPIO value");
        return -1;
    }
    return 0;
}

int gpio_get(gpiod_t *gpio)
{
    int value = gpiod_line_get_value(gpio->line);
    if (value < 0)
    {
        perror("Failed to get GPIO value");
        return -1;
    }
    return value;
}

int gpio_wait_for_event(gpiod_t *gpio, int timeout_ms)
{
    struct timespec timeout;

    if (timeout_ms < 0) {
        return gpiod_line_event_wait(gpio->line, NULL);
    }

    timeout.tv_sec = timeout_ms / 1000;
    timeout.tv_nsec = (timeout_ms % 1000) * 1000000;

    int ret = gpiod_line_event_wait(gpio->line, &timeout);
    if (ret < 0)
    {
        perror("Failed to wait for GPIO event");
        return -1;
    }

    return ret;
}

int gpio_read_event(gpiod_t *gpio)
{
    struct gpiod_line_event event;

    if (gpiod_line_event_read(gpio->line, &event) < 0)
    {
        perror("Failed to read GPIO event");
        return -1;
    }

    return event.event_type;
}

void gpio_close(gpiod_t *gpio)
{
    if (gpio->line)
    {
        gpiod_line_release(gpio->line);
        gpio->line = NULL;
    }

    if (gpio->chip)
    {
        gpiod_chip_close(gpio->chip);
        gpio->chip = NULL;
    }
}