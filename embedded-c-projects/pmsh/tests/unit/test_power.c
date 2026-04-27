#include <stdio.h>
#include "pmsh_power.h"

#define CHECK(c,m) if(!(c)){printf("[FAIL] %s\n",m);return 1;}

int main()
{
    power_init();

    CHECK(power_is_sleeping()==0,"init");

    power_sleep(100);

    power_update(50);
    CHECK(power_is_sleeping()==1,"still sleeping");

    power_update(100);
    CHECK(power_is_sleeping()==0,"wake");

    power_sleep(0);
    CHECK(power_is_sleeping()==0,"zero sleep");

    printf("[PASS] test_power\n");
    return 0;
}
