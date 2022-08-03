#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>

int main(void)
{
    psignal(SIGINT, "SIGINT describe info");
    psignal(SIGALRM, "SIGALRM describe info");
    psignal(SIGQUIT, "SIGQUIT describe info");
    psignal(SIGBUS, "SIGBUS describe info");
    return 0;
}