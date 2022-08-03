#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

void sig_handler(int sig_num)
{
    printf("receive sig_num : %d!\n", sig_num);
}

int main(int argc, char *argv[])
{
    struct sigaction sig = {0};
    int ret = 0;
    int seconds = 0;

    if(argc != 2)
    {
        perror("usage error!\n");
        exit(-1);
    }

    sig.sa_handler = sig_handler;
    sig.sa_flags = 0;

    ret = sigaction(SIGALRM, &sig, NULL);
    if(ret < 0)
    {
        perror("sigaction error!\n");
        exit(-1);
    }

    seconds = atoi(argv[1]);
    printf("alarm : %d seconds!\n", seconds);
    alarm(seconds);

    ret = pause();
    printf("pause ret : %d!\n", ret);

    return 0;
}