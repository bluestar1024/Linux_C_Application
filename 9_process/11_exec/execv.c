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
    char *arg_arr[] = {"ls", "-a", "-l", NULL};
    execv("/bin/ls", arg_arr);
    perror("execv error!\n");
    return 0;
}