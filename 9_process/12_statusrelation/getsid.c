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
    pid_t pid = getpid();
    printf("process ID : %d!\n", pid);
    printf("process group ID : %d!\n", getpgrp());
    printf("process SID : %d!\n", getsid(0));
    printf("process SID : %d!\n", getsid(pid));
    return 0;
}