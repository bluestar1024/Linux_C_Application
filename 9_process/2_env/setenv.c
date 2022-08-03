#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char *argv[])
{
    int ret = 0;
    char *str_ret = NULL;

    if(3 != argc)
    {
        perror("usage error!\n");
        exit(-1);
    }

    ret = setenv(argv[1],argv[2],1);
    if(ret < 0)
    {
        perror("setenv error!\n");
        exit(-1);
    }

    str_ret = getenv(argv[1]);
    if(NULL == str_ret)
    {
        perror("getenv error!\n");
        exit(-1);
    }

    puts(str_ret);
    return 0;
}