#include <stdlib.h>
#include <stdio.h>
#include <barrelfish/barrelfish.h>
#include <barrelfish/spawn_client.h>
int main(int argc, char *argv[])
{
    coreid_t mycore = disp_get_core_id();
    printf("----on core %d",mycore);
    return EXIT_SUCCESS;
}