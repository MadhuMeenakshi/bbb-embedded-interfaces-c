#ifndef PMSH_SENSOR_H
#define PMSH_SENSOR_H

#include <stdint.h>
#include "pmsh_ring.h"

/* Sensor initialization */
void sensor_init(uint32_t seed);

/* Generate one sample (fills struct) */
void sensor_generate(Sample *s, uint32_t ts_ms);

#endif /* PMSH_SENSOR_H */
