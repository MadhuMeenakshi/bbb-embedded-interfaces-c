#include <stdio.h>
#include "pmsh_ring.h"

#define CHECK(c,m) if(!(c)){printf("[FAIL] %s\n",m);return 1;}

int main()
{
    ring_t r;
    Sample s = {0}, out;

    ring_init(&r, RING_DROPNEW);

    CHECK(ring_count(&r)==0,"init count");
    CHECK(ring_pop(&r,&out)==-1,"pop empty");

    /* Fill */
    for(size_t i=0;i<RING_SIZE;i++){
        s.ts_ms=i;
        CHECK(ring_push(&r,&s)==0,"push");
    }

    CHECK(ring_count(&r)==RING_SIZE,"full count");

    /* DROPNEW */
    CHECK(ring_push(&r,&s)==-1,"dropnew reject");
    CHECK(ring_overruns(&r)==1,"overrun count");

    /* Pop all */
    for(size_t i=0;i<RING_SIZE;i++){
        CHECK(ring_pop(&r,&out)==0,"pop ok");
    }

    CHECK(ring_pop(&r,&out)==-1,"empty again");

    /* OVERWRITE */
    ring_init(&r,RING_OVERWRITE);

    for(size_t i=0;i<RING_SIZE+5;i++){
        s.ts_ms=i;
        ring_push(&r,&s);
    }

    CHECK(ring_count(&r)==RING_SIZE,"overwrite size");
    CHECK(ring_overruns(&r)==5,"overwrite count");

    printf("[PASS] test_ring\n");
    return 0;
}
