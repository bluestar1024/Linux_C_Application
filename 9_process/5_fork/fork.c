#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>

int main(void)
{
    pid_t pid = fork();
    switch(pid)
    {
        case -1:
            perror("fork error!\n");
            exit(-1);
        
        case 0:
            puts("child process!");
            printf("parent pid : %d, child pid : %d, pid : %d!\n", getppid(), getpid(), pid);
            puts("child process exit!");
            break;
        
        default:
            puts("parent process!");
            printf("parent pid : %d, child pid : %d!\n", getpid(), pid);
            puts("parent process exit!");
            break;
    }
    return 0;
}