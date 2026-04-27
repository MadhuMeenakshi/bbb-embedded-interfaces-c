#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "pmsh_cmd.h"
#include "pmsh_platform.h"
#include "pmsh_power.h"

/* Limits */
#define MIN_PERIOD_MS 50U
#define MAX_PERIOD_MS 1000U

void cmd_process(const char *cmd,
                 ring_t *ring,
                 uint32_t *period_ms)
{
    if ((cmd == NULL) || (ring == NULL) || (period_ms == NULL))
        return;

    /* -------- STAT -------- */
    if (strncmp(cmd, "STAT", 4) == 0) {
        char out[128];

        snprintf(out, sizeof(out),
                 "Samples:%zu Overruns:%u Period:%u ms\n",
                 ring_count(ring),
                 ring_overruns(ring),
                 *period_ms);

        platform_write(out);
    }

    /* -------- GET -------- */
    else if (strncmp(cmd, "GET", 3) == 0) {
        int n = 0;

        if (sscanf(cmd, "GET %d", &n) != 1 || n <= 0) {
            platform_write("ERR\n");
            return;
        }

        for (int i = n - 1; i >= 0; i--) {
            Sample s;

            if (ring_peek_latest(ring, &s, (size_t)i) == 0) {
                char out[128];

                snprintf(out, sizeof(out),
                         "%u,%d,%d,%d,%d,%u\n",
                         s.ts_ms,
                         s.t_cdeg,
                         s.ax_mg,
                         s.ay_mg,
                         s.az_mg,
                         s.flags);

                platform_write(out);
            }
        }
    }

    /* -------- CFG PERIOD -------- */
    else if (strncmp(cmd, "CFG PERIOD", 10) == 0) {
        int val = 0;

    if (sscanf(cmd, "CFG PERIOD %d", &val) == 1 &&
        val >= 0 &&
        (uint32_t)val >= MIN_PERIOD_MS &&
        (uint32_t)val <= MAX_PERIOD_MS) {

        *period_ms = (uint32_t)val;
        platform_write("OK\n");
    } else {
        platform_write("ERR\n");
    }
    }

    /* -------- CFG OVERRUN -------- */
    else if (strncmp(cmd, "CFG OVERRUN DROPNEW", 20) == 0) {
        ring_set_policy(ring, RING_DROPNEW);
        platform_write("OK\n");
    }

    else if (strncmp(cmd, "CFG OVERRUN OVERWRITE", 21) == 0) {
        ring_set_policy(ring, RING_OVERWRITE);
        platform_write("OK\n");
    }

    /* -------- SLEEP -------- */
    else if (strncmp(cmd, "SLEEP", 5) == 0) {
        int ms = 0;

        if (sscanf(cmd, "SLEEP %d", &ms) == 1 && ms > 0) {
            power_sleep((uint32_t)ms);
            platform_write("SLEEPING\n");
        } else {
            platform_write("ERR\n");
        }
    }

    /* -------- UNKNOWN -------- */
    else {
        platform_write("UNKNOWN CMD\n");
    }
}
