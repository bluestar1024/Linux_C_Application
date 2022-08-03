#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char *argv[])
{
    char *str_ret = NULL;

    if(2 != argc)
    {
        perror("usage error!\n");
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