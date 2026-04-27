#include "pmsh_sensor.h"

/* Fault injection controls (compile-time) */
#ifdef PMSH_FAULT_SENSOR
#define SENSOR_FAIL_PERIOD 10U
#endif

#ifdef PMSH_FAULT_JITTER
#define JITTER_MAX 5U
#endif

/* Internal deterministic state */
static uint32_t g_state = 1U;

/* Simple LCG (deterministic pseudo-random) */
static uint32_t prng_next(void)
{
    g_state = (1103515245U * g_state + 12345U);
    return g_state;
}

/* Initialization */
void sensor_init(uint32_t seed)
{
    if (seed == 0U) {
        seed = 1U; /* avoid zero lock */
    }
    g_state = seed;
}

/* Generate deterministic sensor data */
void sensor_generate(Sample *s, uint32_t ts_ms)
{
    if (s == NULL)
        return;

    s->ts_ms = ts_ms;
    s->flags = 0U;

    /* Base deterministic values */
    int32_t temp = (int32_t)(prng_next() % 16500) - 4000;  /* -40.00°C to +125.00°C (centi-deg) */

    int32_t ax = (int32_t)(prng_next() % 32000) - 16000;   /* approx ±16g */
    int32_t ay = (int32_t)(prng_next() % 32000) - 16000;
    int32_t az = (int32_t)(prng_next() % 32000) - 16000;

#ifdef PMSH_FAULT_SENSOR
    static uint32_t count = 0;
    count++;

    if ((count % SENSOR_FAIL_PERIOD) == 0U) {
        /* Inject failure */
        s->flags |= (1U << 2); /* SENSOR_FAIL */
        temp = 20000;          /* invalid temp */
    }
#endif

#ifdef PMSH_FAULT_JITTER
    uint32_t jitter = prng_next() % (JITTER_MAX + 1U);
    temp += (int32_t)jitter;
#endif

    /* Assign */
    s->t_cdeg = (int16_t)temp;
    s->ax_mg = (int16_t)ax;
    s->ay_mg = (int16_t)ay;
    s->az_mg = (int16_t)az;

    /* Range checks */

    /* Temperature: -40 to +125 °C → -4000 to 12500 centi-deg */
    if ((temp < -4000) || (temp > 12500)) {
        s->flags |= (1U << 0); /* TEMP_RANGE */
    }

    /* Acceleration: |a| <= 16000 mg (≈16g) */
    if ((ax < -16000) || (ax > 16000) ||
        (ay < -16000) || (ay > 16000) ||
        (az < -16000) || (az > 16000)) {
        s->flags |= (1U << 1); /* ACCEL_RANGE */
    }
}
