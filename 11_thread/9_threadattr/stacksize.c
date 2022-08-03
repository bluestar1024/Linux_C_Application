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
    puts("hello world!");
    return (void *)0;
}

int main(void)
{
    pthread_attr_t attr;
    size_t stacksize;
    pthread_t tid;
    int ret = 0;

    pthread_attr_init(&attr);
    pthread_attr_getstacksize(&attr, &stacksize);
    printf("stack size : %lu!\n", stacksize);
    pthread_attr_setstacksize(&attr, 4096);
    pthread_attr_getstacksize(&attr, &stacksize);
    printf("stack size : %lu!\n", stacksize);

    ret = pthread_create(&tid, &attr, child_thread, NULL);
    if(ret)
    {
        perror("create thread error");
        exit(-1);
    }

    ret = pthread_join(tid, NULL);
    if(ret)
    {
        perror("recycle thread error");
        exit(-1);
    }

    pthread_attr_destroy(&attr);
    return 0;
}