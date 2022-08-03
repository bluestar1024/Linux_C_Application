#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

int main(int argc, char *argv[])
{
    int ret = 0;

    if(2 != argc)
    {
        perror("usage error!\n");
        exit(-1);
    }

    ret = system(argv[1]);
    if((-1 == ret) || (127 == WEXITSTATUS(ret)))
    {
        perror("system error!\n");
        exit(-1);
    }

    return 0;
}