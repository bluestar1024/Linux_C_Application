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
    printf("child thread : pid<%d>, tid<%lu>\n", getpid(), pthread_self());
    return (void *)0;
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

    printf("main thread : pid<%d>, tid<%lu>\n", getpid(), pthread_self());
    sleep(1);
    return 0;
}