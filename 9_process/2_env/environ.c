#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>

extern char ** environ;

int main(void)
{
    int i = 0;
    for(; NULL != environ[i]; i++)
        puts(environ[i]);
    return 0;
}