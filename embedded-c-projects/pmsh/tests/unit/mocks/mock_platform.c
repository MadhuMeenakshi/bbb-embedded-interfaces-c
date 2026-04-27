#include <string.h>
#include "mock_platform.h"

/* capture output */
static char buf[512];

void platform_write(const char *s)
{
    strcat(buf,s);
}

void mock_reset(void)
{
    memset(buf,0,sizeof(buf));
}

const char* mock_output(void)
{
    return buf;
}
