#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char *argv[])
{
    int ret = 0;
    char name[50] = {0};
    char *str_ret = NULL;
    int i = 0;

    if(2 != argc)
    {
        perror("usage error!\n");
        exit(-1);
    }

    ret = putenv(argv[1]);
    if(0 != ret)
    {
        perror("putenv error!\n");
        exit(-1);
    }

    for(; argv[1][i] != '='; i++)
        name[i] = argv[1][i];

    str_ret = getenv(name);
    if(NULL == str_ret)
    {
        perror("getenv error!\n");
        exit(-1);
    }

    puts(str_ret);
    return 0;
}