#include <stdint.h>
#include <stdio.h>

#include "pmsh_ring.h"
#include "pmsh_sensor.h"
#include "pmsh_platform.h"
#include "pmsh_cmd.h"
#include "pmsh_power.h"

/* Configuration */
#define DEFAULT_PERIOD_MS 100U

/* Global system state */
static ring_t g_ring;
static uint32_t g_period_ms = DEFAULT_PERIOD_MS;
static uint32_t g_last_sample_time = 0U;

static void print_cli_help(void)
{
    platform_write("\n=== PMSH CLI ===\n");
    platform_write("Available Commands:\n");

    platform_write("STAT\n");
    platform_write("    Show system status\n");

    platform_write("GET <n>\n");
    platform_write("    Get last n samples\n");

    platform_write("CFG PERIOD <50-1000>\n");
    platform_write("    Set sampling period in milliseconds\n");

    platform_write("CFG OVERRUN <DROPNEW|OVERWRITE>\n");
    platform_write("    Set buffer overrun policy\n");

    platform_write("SLEEP <ms>\n");
    platform_write("    Pause sampling for given time\n");

    platform_write("\nExamples:\n");
    platform_write("STAT\n");
    platform_write("GET 5\n");
    platform_write("CFG PERIOD 200\n");
    platform_write("CFG OVERRUN OVERWRITE\n");
    platform_write("SLEEP 500\n");

    platform_write("=================\n\n");
}

/* ========================= MAIN ========================= */

int main(void)
{
    char line[128];

    /* -------- INIT -------- */
    ring_init(&g_ring, RING_OVERWRITE);
    sensor_init(1234U);
    power_init();
    
    print_cli_help();

    g_last_sample_time = platform_get_time_ms();

    /* -------- MAIN LOOP -------- */
    while (1)
    {
        uint32_t now = platform_get_time_ms();

        /* Update power state */
        power_update(now);

        /* -------- SAMPLING (ISR-like) -------- */
        /* -------- SAMPLING (fixed periodic) -------- */
    while (!power_is_sleeping() &&
           ((now - g_last_sample_time) >= g_period_ms))
    {
        Sample s;

        /* Move to next scheduled time */
        g_last_sample_time += g_period_ms;

        /* Generate sample at exact scheduled time */
        sensor_generate(&s, g_last_sample_time);

        (void)ring_push(&g_ring, &s);
    }

        /* -------- CLI PROCESSING -------- */
        if (platform_read_line(line, sizeof(line)))
        {
            cmd_process(line, &g_ring, &g_period_ms);
        }

        /* -------- CPU RELIEF -------- */
        platform_sleep_ms(10U);
    }

    return 0;
}
