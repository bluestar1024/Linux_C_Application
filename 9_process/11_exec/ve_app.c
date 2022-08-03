#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

extern char **environ;

int main(int argc, char *argv[])
{
    int i = 0;
    puts("arg:");
    for(; i < argc; i++)
        printf("argv[%d] : %s!\n", i, argv[i]);
    puts("env:");
    for(i = 0; NULL != environ[i]; i++)
        printf("environ[%d] : %s!\n", i, environ[i]);
    return 0;
}