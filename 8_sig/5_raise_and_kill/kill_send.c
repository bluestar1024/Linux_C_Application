#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>

int main(int argc,char *argv[])
{
    int pid = 0;
    if(argc != 2)
    {
        perror("usage error!\n");
        exit(-1);
    }

    pid = atoi(argv[1]);
    while(1)
    {
        sleep(2);
        kill(pid, SIGINT);
    }
    return 0;
}