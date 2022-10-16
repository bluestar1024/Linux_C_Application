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

static pthread_spinlock_t lock;
static int num = 0;

void *child_thread(void *arg)
{
    int loops = *(int *)arg;
    int i = 0, count = 0;

    for(; i < loops; i++)
    {
        pthread_spin_lock(&lock);

        count = num;
        count++;
        num = count;

        pthread_spin_unlock(&lock);
    }
    return (void *)0;
}

int main(int argc, char *argv[])
{
    pthread_t tid1, tid2;
    int ret = 0;
    int loops = 0;

    if(2 != argc)
        loops = 10000000;
    else
        loops = atoi(argv[1]);

    pthread_spin_init(&lock, PTHREAD_PROCESS_PRIVATE);

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

    ret = pthread_join(tid1, NULL);
    if(ret)
    {
        perror("recycle thread1 error");
        exit(-1);
    }

    ret = pthread_join(tid2, NULL);
    if(ret)
    {
        perror("recycle thread2 error");
        exit(-1);
    }

    pthread_spin_destroy(&lock);

    printf("num : %d!\n", num);
    return 0;
}