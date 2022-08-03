#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

int main(int argc, char *argv[])
{
    char *arg_arr[5] = {[4] = NULL};
    char *env_arr[5] = {[4] = NULL};

    if(2 != argc)
    {
        perror("usage error!\n");
        exit(-1);
    }

    arg_arr[0] = argv[1];
    arg_arr[1] = "hello";
    arg_arr[2] = "world";
    arg_arr[3] = NULL;

    env_arr[0] = "NAME=app";
    env_arr[1] = "AGE=30";
    env_arr[2] = "SEX=man";
    env_arr[3] = NULL;

    execve(argv[1], arg_arr, env_arr);
    perror("execve error!\n");
    return 0;
}