#include <stdio.h>
#include <stdlib.h>

int run(const char *cmd)
{
    int r = system(cmd);
    if(r!=0){
        printf("[FAIL] %s\n",cmd);
    }
    return r;
}

int main()
{
    int fail = 0;

    printf("\n===== UNIT TEST REPORT =====\n");

    fail |= run("./tests/unit/test_ring");
    fail |= run("./tests/unit/test_sensor");
    fail |= run("./tests/unit/test_power");
    fail |= run("./tests/unit/test_cmd");

    if(fail==0)
        printf("\nALL TESTS PASSED ✅\n");
    else
        printf("\nSOME TESTS FAILED ❌\n");

    printf("============================\n");

    return fail;
}
