#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

void sig_handler(int sig_num)
{
    printf("receive sig_num : %d!\n", sig_num);
}

int main(void)
{
    struct sigaction sig ={0};
    int ret = 0;
    sigset_t sig_set;

    sig.sa_handler = sig_handler;
    sig.sa_flags = 0;

    ret = sigaction(SIGINT, &sig, NULL);
    if(ret < 0)
    {
        perror("sigaction error!\n");
        exit(-1);
    }

    sigemptyset(&sig_set);
    sigaddset(&sig_set, SIGINT);

    ret = sigprocmask(SIG_BLOCK, &sig_set, NULL);
    if(ret < 0)
    {
        perror("sigprocmask error!\n");
        exit(-1);
    }

    raise(SIGINT);

    sleep(2);
    printf("sleep finish!\n");

    ret = sigprocmask(SIG_UNBLOCK, &sig_set, NULL);
    if(ret < 0)
    {
        perror("sigprocmask error!\n");
        exit(-1);
    }
    return 0;
}