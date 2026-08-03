#ifndef GPIO_H
#define GPIO_H

#include <gpiod.h>

typedef struct {
    struct gpiod_chip *chip;
    struct gpiod_line *line;
} gpiod_t;

typedef enum {
    GPIO_INPUT,
    GPIO_OUTPUT
} gpio_direction;

// map event types
#define GPIO_EVENT_RISING_EDGE  GPIOD_LINE_EVENT_RISING_EDGE
#define GPIO_EVENT_FALLING_EDGE GPIOD_LINE_EVENT_FALLING_EDGE

int gpio_init(gpiod_t *gpio, int chip_num, int line_num, gpio_direction direction);
int gpio_set(gpiod_t *gpio, int val);
int gpio_get(gpiod_t *gpio);
int gpio_wait_for_event(gpiod_t *gpio, int timeout_ms);
int gpio_read_event(gpiod_t *gpio);
void gpio_close(gpiod_t *gpio);

#endif