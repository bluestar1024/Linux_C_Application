#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

int main(void)
{
    int i = 0;
    int status = 0;
    pid_t pid = 0;

    for(; i < 3; i++)
    {
        switch(fork())
        {
            case -1:
                perror("fork error!\n");
                exit(-1);
            
            case 0:
                printf("child %d pid : %d!\n", i, getpid());
                sleep(1);
                _exit(i);
            
            default:
                break;
        }
    }

    for(;;)
    {
        pid = waitpid(-1, &status, WNOHANG);
        if(-1 == pid)
        {
            if(ECHILD == errno)
                break;
            else
            {
                perror("waitpid error!\n");
                exit(-1);
            }
        }
        else if(0 == pid)
            continue;
        else
            printf("child pid : %d, status : %d!\n", pid, WEXITSTATUS(status));
    }
    return 0;
}