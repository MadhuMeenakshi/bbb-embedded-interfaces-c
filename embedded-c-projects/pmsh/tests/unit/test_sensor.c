#include <stdio.h>
#include "pmsh_sensor.h"

#define CHECK(c,m) if(!(c)){printf("[FAIL] %s\n",m);return 1;}

int main()
{
    Sample s;

    sensor_init(123);

    sensor_generate(&s,1000);

    CHECK(s.ts_ms==1000,"timestamp");

    CHECK(s.t_cdeg>=-4000 && s.t_cdeg<=12500,"temp range");
    CHECK(s.ax_mg>=-16000 && s.ax_mg<=16000,"accel x");
    CHECK(s.ay_mg>=-16000 && s.ay_mg<=16000,"accel y");
    CHECK(s.az_mg>=-16000 && s.az_mg<=16000,"accel z");

    CHECK(s.flags<=0x07,"flags");

    printf("[PASS] test_sensor\n");
    return 0;
}
