#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>

int main(void)
{
    pid_t pid = vfork();
    switch(pid)
    {
        case -1:
            perror("vfork error!\n");
            exit(-1);
        
        case 0:
            puts("The child process!");
            puts("The child process executes before the parent process!");
            _exit(-1);
        
        default:
            puts("The parent process!");
            puts("The parent process executes before the child process!");
            break;
    }
    return 0;
}