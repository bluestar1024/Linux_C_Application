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

static pthread_mutex_t mutex;
static pthread_cond_t cond;
static int num = 0;

void *child_thread(void *arg)
{
    while(1)
    {
        pthread_mutex_lock(&mutex);
        while(num <= 0)
            pthread_cond_wait(&cond, &mutex);
        while(num > 0)
            num--;
        pthread_mutex_unlock(&mutex);
    }
    return (void *)0;
}

int main(int argc, char *argv[])
{
    pthread_t tid;
    int ret = 0;
    int loops = 0;
    int i = 0;

    if(2 != argc)
        loops = 10000000;
    else
        loops = atoi(argv[1]);

    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);

    ret = pthread_create(&tid, NULL, child_thread, NULL);
    if(ret)
    {
        perror("create thread error");
        exit(-1);
    }

    for(; i < loops; i++)
    {
        pthread_mutex_lock(&mutex);
        num++;
        pthread_mutex_unlock(&mutex);
        pthread_cond_signal(&cond);
    }

    ret = pthread_cancel(tid);
    if(ret)
    {
        perror("cancel thread error");
        exit(-1);
    }

    ret = pthread_join(tid, NULL);
    if(ret)
    {
        perror("recycle thread error");
        exit(-1);
    }

    pthread_cond_destroy(&cond);
    pthread_mutex_destroy(&mutex);

    printf("num : %d!\n", num);
    return 0;
}