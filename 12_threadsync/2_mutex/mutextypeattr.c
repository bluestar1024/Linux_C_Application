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
static pthread_mutexattr_t attr;
static int num = 0;

void mutex_init(void)
{
    int type;

    pthread_mutexattr_init(&attr);
    pthread_mutexattr_gettype(&attr, &type);
    if(PTHREAD_MUTEX_DEFAULT == type)
        puts("default type attr!");
    else if(PTHREAD_MUTEX_NORMAL == type)
        puts("default type attr!");
    else
        puts("type attr unknow!");
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_NORMAL);
    pthread_mutexattr_gettype(&attr, &type);
    if(PTHREAD_MUTEX_DEFAULT == type)
        puts("default type attr!");
    else if(PTHREAD_MUTEX_NORMAL == type)
        puts("default type attr!");
    else
        puts("type attr unknow!");
    pthread_mutex_init(&mutex, &attr);
}
void mutex_destroy(void)
{
    pthread_mutexattr_destroy(&attr);
    pthread_mutex_destroy(&mutex);
}

void *child_thread(void *arg)
{
    int loops = *(int *)arg;
    int count = 0, i = 0;

    for(; i < loops; i++)
    {
        pthread_mutex_lock(&mutex);

        count = num;
        count++;
        num = count;

        pthread_mutex_unlock(&mutex);
    }
}

int main(int argc, char *argv[])
{
    int loops = 0;
    pthread_t tid1, tid2;
    int ret = 0;

    if(2 != argc)
        loops = 10000000;
    else
        loops = atoi(argv[1]);
    
    mutex_init();

    ret = pthread_create(&tid1, NULL, child_thread, &loops);
    if(ret)
    {
        perror("create thread1 error");
        exit(-1);
    }

    ret = pthread_create(&tid2, NULL, child_thread, &loops);
    if(ret)
    {
        perror("create thread2 error");
        exit(-1);
    }

    ret = pthread_join(tid1 ,NULL);
    if(ret)
    {
        perror("recycle thread1 error");
        exit(-1);
    }

    ret = pthread_join(tid2 ,NULL);
    if(ret)
    {
        perror("recycle thread2 error");
        exit(-1);
    }

    printf("num : %d!\n", num);

    mutex_destroy();
    return 0;
}