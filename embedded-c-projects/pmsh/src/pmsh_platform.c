#define _POSIX_C_SOURCE 200809L

#include "pmsh_platform.h"

#include <stdio.h>
#include <time.h>

/* Get monotonic time in milliseconds */
uint32_t platform_get_time_ms(void)
{
    struct timespec ts;

    clock_gettime(CLOCK_MONOTONIC, &ts);

    return (uint32_t)((ts.tv_sec * 1000U) + (ts.tv_nsec / 1000000U));
}

/* Sleep using nanosleep (POSIX compliant) */
void platform_sleep_ms(uint32_t ms)
{
    struct timespec ts;

    ts.tv_sec = ms / 1000U;
    ts.tv_nsec = (ms % 1000U) * 1000000U;

    nanosleep(&ts, NULL);
}

/* Read line from stdin */
int platform_read_line(char *buf, uint32_t max_len)
{
    if (buf == NULL || max_len == 0U)
        return 0;

    if (fgets(buf, (int)max_len, stdin) != NULL) {
        return 1;
    }

    return 0;
}

/* Write string to stdout */
void platform_write(const char *str)
{
    if (str == NULL)
        return;

    printf("%s", str);
    fflush(stdout);
}
