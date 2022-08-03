#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

extern char **environ;

int main(void)
{
    char *arg_arr[] = {"ls", "-a", "-l", NULL};
    execvpe("ls", arg_arr, environ);
    perror("execvpe error!\n");
    return 0;
}