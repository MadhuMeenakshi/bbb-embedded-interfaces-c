#include "pmsh_power.h"

/* Internal state */
static uint8_t g_sleeping = 0U;
static uint32_t g_wakeup_time = 0U;

void power_init(void)
{
    g_sleeping = 0U;
    g_wakeup_time = 0U;
}

void power_sleep(uint32_t duration_ms)
{
    if (duration_ms == 0U)
        return;

    g_sleeping = 1U;
    g_wakeup_time += duration_ms; /* caller ensures correct base time */
}

void power_update(uint32_t now_ms)
{
    if (g_sleeping && (now_ms >= g_wakeup_time)) {
        g_sleeping = 0U;
    }
}

uint8_t power_is_sleeping(void)
{
    return g_sleeping;
}
