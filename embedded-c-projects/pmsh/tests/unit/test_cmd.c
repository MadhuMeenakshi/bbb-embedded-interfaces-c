#include <stdio.h>
#include <string.h>
#include "pmsh_cmd.h"
#include "pmsh_ring.h"
#include "mock_platform.h"

#define CHECK(c,m) if(!(c)){printf("[FAIL] %s\n",m);return 1;}

int main()
{
    ring_t r;
    uint32_t period = 100;

    ring_init(&r,RING_OVERWRITE);

    /* CFG valid */
    mock_reset();
    cmd_process("CFG PERIOD 200",&r,&period);
    CHECK(period==200,"cfg period");
    CHECK(strstr(mock_output(),"OK"),"cfg output");

    /* CFG invalid */
    mock_reset();
    cmd_process("CFG PERIOD 10",&r,&period);
    CHECK(strstr(mock_output(),"ERR"),"invalid");

    /* STAT */
    mock_reset();
    cmd_process("STAT",&r,&period);
    CHECK(strstr(mock_output(),"Samples"),"stat");

    /* GET invalid */
    mock_reset();
    cmd_process("GET -1",&r,&period);
    CHECK(strstr(mock_output(),"ERR"),"get invalid");

    /* UNKNOWN */
    mock_reset();
    cmd_process("XYZ",&r,&period);
    CHECK(strstr(mock_output(),"UNKNOWN"),"unknown");

    printf("[PASS] test_cmd\n");
    return 0;
}
