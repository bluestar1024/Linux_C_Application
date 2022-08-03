#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>

void bye(void)
{
    printf("goodbye!\n");
}

int main(void)
{
    int ret = 0;
    ret = atexit(bye);
    if(ret)
    {
        perror("atexit error!\n");
        exit(-1);
    }
    return 0;
}