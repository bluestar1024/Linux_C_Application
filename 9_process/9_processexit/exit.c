#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>

int main(void)
{
    printf("hello world!");
    switch(fork())
    {
        case -1:
            perror("fork error!\n");
            exit(-1);
        
        case 0:
            exit(0);
        
        default:
            exit(0);
    }
    return 0;
}