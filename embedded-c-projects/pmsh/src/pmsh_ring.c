#include "pmsh_ring.h"

/* Internal helper: increment index with wrap */
static inline size_t next_index(size_t idx)
{
    return (idx + 1U) % RING_SIZE;
}

void ring_init(ring_t *r, ring_policy_t policy)
{
    r->head = 0U;
    r->tail = 0U;
    r->count = 0U;
    r->overrun_count = 0U;
    r->policy = policy;
}

/*
 * Push:
 * - O(1)
 * - Safe for single producer
 */
int ring_push(ring_t *r, const Sample *s)
{
    if (r->count == RING_SIZE) {
        r->overrun_count++;

        if (r->policy == RING_DROPNEW) {
            return -1;
        }

        /* OVERWRITE: discard oldest */
        r->tail = next_index(r->tail);
        r->count--;
    }

    r->buffer[r->head] = *s;
    r->head = next_index(r->head);
    r->count++;

    return 0;
}

/*
 * Pop:
 * - O(1)
 * - Safe for single consumer
 */
int ring_pop(ring_t *r, Sample *s)
{
    if (r->count == 0U) {
        return -1;
    }

    *s = r->buffer[r->tail];
    r->tail = next_index(r->tail);
    r->count--;

    return 0;
}

/*
 * Peek latest Nth sample without removing
 * index_from_latest = 0 → latest
 */
int ring_peek_latest(const ring_t *r, Sample *out, size_t index_from_latest)
{
    if ((r->count == 0U) || (index_from_latest >= r->count)) {
        return -1;
    }

    /* Calculate position from head */
    size_t idx = (r->head + RING_SIZE - 1U - index_from_latest) % RING_SIZE;
    *out = r->buffer[idx];

    return 0;
}

size_t ring_count(const ring_t *r)
{
    return r->count;
}

uint32_t ring_overruns(const ring_t *r)
{
    return r->overrun_count;
}

void ring_set_policy(ring_t *r, ring_policy_t policy)
{
    r->policy = policy;
}
