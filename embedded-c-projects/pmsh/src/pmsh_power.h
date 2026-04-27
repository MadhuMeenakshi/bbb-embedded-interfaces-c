#ifndef PMSH_POWER_H
#define PMSH_POWER_H

#include <stdint.h>

/* Initialize power module */
void power_init(void);

/* Enter sleep for ms */
void power_sleep(uint32_t duration_ms);

/* Check if system is sleeping */
uint8_t power_is_sleeping(void);

/* Update state (call periodically) */
void power_update(uint32_t now_ms);

#endif /* PMSH_POWER_H */
