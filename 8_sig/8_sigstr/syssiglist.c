#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

int main(void)
{
    printf("SIGINT describe info : %s\n", sys_siglist[SIGINT]);
    printf("SIGALRM describe info : %s\n", sys_siglist[SIGALRM]);
    printf("SIGQUIT describe info : %s\n", sys_siglist[SIGQUIT]);
    printf("SIGBUS describe info : %s\n", sys_siglist[SIGBUS]);
    return 0;
}