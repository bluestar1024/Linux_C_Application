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
    sleep(2);
    puts("child thread end!");
    return NULL;
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

    ret = pthread_detach(tid);
    if(ret)
    {
        perror("detach thread error");
        exit(-1);
    }

    sleep(1);

    ret = pthread_join(tid, NULL);
    if(ret)
        fprintf(stderr, "pthread_join error : %s!\n", strerror(ret));

    pthread_exit(NULL);
}