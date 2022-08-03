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
    printf("process group ID : %d!\n", getpgid(0));
    printf("process group ID : %d!\n", getpgid(pid));
    return 0;
}