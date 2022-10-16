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

static pthread_rwlock_t rwlock;
static int num = 0;

void *read_thread(void *arg)
{
    int id = *(int *)arg;
    int i = 0;

    for(; i < 10; i++)
    {
        pthread_rwlock_rdlock(&rwlock);
        printf("read_thread<%d> : num=%d!\n", id, num);
        pthread_rwlock_unlock(&rwlock);
        sleep(1);
    }
    return (void *)0;
}
void *write_thread(void *arg)
{
    int id = *(int *)arg;
    int i = 0;

    for(; i < 10; i++)
    {
        pthread_rwlock_wrlock(&rwlock);
        printf("write_thread<%d> : num=%d!\n", id, num += 20);
        pthread_rwlock_unlock(&rwlock);
        sleep(1);
    }
    return (void *)0;
}

int main(void)
{
    pthread_t tid[10];
    int ret = 0;
    int i = 0;
    int nums[] = {0, 1, 2, 3, 4};

    pthread_rwlock_init(&rwlock, NULL);

    for(; i < 5; i++)
    {
        ret = pthread_create(tid + i, NULL, read_thread, nums + i);
        if(ret)
        {
            printf("create thread%d error!\n", i);
            exit(-1);
        }
    }

    for(; i < 10; i++)
    {
        ret = pthread_create(tid + i, NULL, write_thread, nums + i - 5);
        if(ret)
        {
            printf("create thread%d error!\n", i);
            exit(-1);
        }
    }

    for(i = 0; i < 10; i++)
    {
        ret = pthread_join(tid[i], NULL);
        if(ret)
        {
            printf("recycle thread%d error!\n", i);
            exit(-1);
        }
    }

    pthread_rwlock_destroy(&rwlock);
    return 0;
}