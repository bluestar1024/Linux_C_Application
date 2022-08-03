#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>

void sig_handler(int sig_num)
{
    printf("receive sig_num : %d!\n", sig_num);
}

int main(void)
{
    struct sigaction sig = {0};
    int ret = 0;

    sig.sa_handler = sig_handler;
    sig.sa_flags = 0;

    ret = sigaction(SIGABRT, &sig, NULL);
    if(ret < 0)
    {
        perror("sigaction error!\n");
        exit(-1);
    }

    puts("sleeping...");
    sleep(2);
    puts("sleep finish!");

    abort();
    
    while(1);
    return 0;
}