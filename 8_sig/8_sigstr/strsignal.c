#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>

int main(void)
{
    printf("SIGINT describe info : %s\n", strsignal(SIGINT));
    printf("SIGALRM describe info : %s\n", strsignal(SIGALRM));
    printf("SIGQUIT describe info : %s\n", strsignal(SIGQUIT));
    printf("SIGBUS describe info : %s\n", strsignal(SIGBUS));
    return 0;
}