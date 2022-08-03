#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/file.h>
#include <pthread.h>

void *child_thread(void *arg)
{
    puts("child thread start!");
    sleep(1);
    puts("child thread end!");
    pthread_exit(NULL);
}

int main(void)
{
    pthread_t tid;
    int ret = 0;

    ret = pthread_create(&tid, NULL, child_thread, NULL);
    if(ret)
    {
        perror("create thread error");
        exit(-1);
    }

    puts("main thread exit!");
    pthread_exit(NULL);
    return 0;
}