#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char *argv[])
{
    sigval_t sig_val;
    int pid;
    int sig;
    int ret = 0;
    
    if(argc != 3)
    {
        perror("usage error!\n");
        exit(-1);
    }

    pid = atoi(argv[1]);
    sig = atoi(argv[2]);
    sig_val.sival_int = 10;
    printf("pid : %d, sig : %d!\n", pid, sig);

    ret = sigqueue(pid, sig, sig_val);
    if(ret < 0)
    {
        perror("sigqueue error!\n");
        exit(-1);
    }

    puts("send sig success!");
    return 0;
}