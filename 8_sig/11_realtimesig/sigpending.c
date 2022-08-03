#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>

int main(void)
{
    sigset_t sig_set, sig_mask;
    int ret = 0;

    sigemptyset(&sig_mask);
    sigaddset(&sig_mask, SIGINT);
    sigprocmask(SIG_BLOCK, &sig_mask, NULL);

    sleep(10);

    sigemptyset(&sig_set);
    sigpending(&sig_set);
    ret = sigismember(&sig_set, SIGINT);
    if(1 == ret)
        printf("wait queue have SIGINT!\n");
    else if(-1 == ret)
        printf("sigismember error!\n");
    //else if(0 == ret)
        //printf("wait queue not have SIGINT!\n");
    
    return 0;
}