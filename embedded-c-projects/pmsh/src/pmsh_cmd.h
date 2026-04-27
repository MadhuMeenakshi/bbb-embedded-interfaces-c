#ifndef PMSH_CMD_H
#define PMSH_CMD_H

#include <stdint.h>
#include "pmsh_ring.h"

/* Process one command line */
void cmd_process(const char *cmd,
                 ring_t *ring,
                 uint32_t *period_ms);

#endif /* PMSH_CMD_H */
