#ifndef PMSH_RING_H
#define PMSH_RING_H

#include <stdint.h>
#include <stddef.h>

/* Configuration */
#define RING_SIZE 128U

/* Overrun policy */
typedef enum {
    RING_DROPNEW = 0,
    RING_OVERWRITE
} ring_policy_t;

/* Sample structure */
typedef struct {
    uint32_t ts_ms;
    int16_t t_cdeg;
    int16_t ax_mg;
    int16_t ay_mg;
    int16_t az_mg;
    uint16_t flags;
} Sample;

/* Ring buffer */
typedef struct {
    Sample buffer[RING_SIZE];
    size_t head;    /* write index */
    size_t tail;    /* read index */
    size_t count;   /* number of valid elements */
    uint32_t overrun_count;
    ring_policy_t policy;
} ring_t;

/* Initialization */
void ring_init(ring_t *r, ring_policy_t policy);

/* Producer (ISR-like) */
int ring_push(ring_t *r, const Sample *s);

/* Consumer */
int ring_pop(ring_t *r, Sample *s);

/* Non-destructive read (for GET command) */
int ring_peek_latest(const ring_t *r, Sample *out, size_t index_from_latest);

/* Utilities */
size_t ring_count(const ring_t *r);
uint32_t ring_overruns(const ring_t *r);
void ring_set_policy(ring_t *r, ring_policy_t policy);

#endif /* PMSH_RING_H */
