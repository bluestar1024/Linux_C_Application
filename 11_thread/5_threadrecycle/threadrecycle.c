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
    pthread_exit((void *)10);
}

int main(void)
{
    pthread_t tid;
    int ret = 0;
    void *tret = NULL;

    ret = pthread_create(&tid, NULL, child_thread, NULL);
    if(ret)
    {
        perror("create thread error");
        exit(-1);
    }

    ret = pthread_join(tid, &tret);
    if(ret)
    {
        perror("recycle thread error");
        exit(-1);
    }

    printf("child thread ret : %ld!\n", (long)tret);
    return 0;
}