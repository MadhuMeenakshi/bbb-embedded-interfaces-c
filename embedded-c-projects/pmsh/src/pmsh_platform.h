#ifndef PMSH_PLATFORM_H
#define PMSH_PLATFORM_H

#include <stdint.h>

/* Get monotonic time in milliseconds */
uint32_t platform_get_time_ms(void);

/* Sleep for given milliseconds */
void platform_sleep_ms(uint32_t ms);

/* Read one line from stdin (returns 1 if line read, 0 otherwise) */
int platform_read_line(char *buf, uint32_t max_len);

/* Write string to stdout */
void platform_write(const char *str);

#endif /* PMSH_PLATFORM_H */
