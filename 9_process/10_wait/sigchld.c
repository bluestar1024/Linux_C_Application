#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

void wait_child(int sig_num)
{
    int status = 0;
    int pid = 0;

    printf("receive sig_num : %d!\n", sig_num);

    while((pid = waitpid(-1, &status, WNOHANG)) > 0)
        printf("child pid : %d, status : %d!\n", pid, WEXITSTATUS(status));
}

int main(void)
{
    struct sigaction sig = {0};
    int ret = 0;
    int i = 0;

    sigemptyset(&sig.sa_mask);
    sig.sa_handler = wait_child;
    sig.sa_flags = 0;
    ret = sigaction(SIGCHLD, &sig, NULL);
    if(ret < 0)
    {
        perror("sigaction error!\n");
        exit(-1);
    }

    for(; i < 3; i++)
    {
        switch(fork())
        {
            case -1:
                perror("fork error!\n");
                exit(-1);
            
            case 0:
                printf("child %d pid %d create finish!\n", i, getpid());
                sleep(1);
                printf("child %d pid %d exit!\n", i, getpid());
                _exit(i);
            
            default:
                break;
        }
    }

    sleep(2);
    puts(".....");
    sleep(2);
    puts(".....");
    sleep(2);
    puts(".....");
    return 0;
}