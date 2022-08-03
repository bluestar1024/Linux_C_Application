#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(void)
{
    pid_t pid;
    int i = 0;

    pid = fork();
    if(pid < 0)
    {
        perror("fork error!\n");
        exit(-1);
    }
    else if(pid > 0)
    {
        printf("parent process ID : %d!\n", getpid());
        printf("parent process group ID : %d!\n", getpgrp());
        printf("parent process SID : %d!\n", getsid(0));

        exit(0);
    }
    
    puts("......");
    sleep(1);
    printf("process ID : %d!\n", getpid());
    printf("process group ID : %d!\n", getpgrp());
    printf("process SID : %d!\n", getsid(0));
    printf("parent process ID : %d!\n", getppid());
    
    if(setsid() < 0)
    {
        perror("setsid error!\n");
        exit(-1);
    }

    if(chdir("/") < 0)
    {
        perror("chdir error!\n");
        exit(-1);
    }

    umask(0);

    for(; i < sysconf(_SC_OPEN_MAX); i++)
        close(i);
    
    open("/dev/null", O_RDWR);
    dup(0);
    dup(0);

    signal(SIGCHLD, SIG_IGN);

    while(1)
    {
        sleep(1);
        puts("daemoning....");
    }

    return 0;
}