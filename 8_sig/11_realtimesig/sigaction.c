#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>

void sig_handler(int sig_num, siginfo_t *sig_info, void *context)
{
    sigval_t sig_data = sig_info->si_value;

    printf("receive sig_num : %d!\n", sig_num);
    printf("receive sig_data : %d!\n", sig_data.sival_int);
}

int main(int argc, char *argv[])
{
    struct sigaction sig = {0};
    int num = 0;
    int ret = 0;

    if(argc != 2)
    {
        perror("usage error!\n");
        exit(-1);
    }

    sig.sa_sigaction = sig_handler;
    sig.sa_flags = SA_SIGINFO;

    num = atoi(argv[1]);

    ret = sigaction(num, &sig, NULL);
    if(ret < 0)
    {
        perror("sigaction error!\n");
        exit(-1);
    }

    while(1);
    return 0;
}