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
    int ret = pthread_detach(pthread_self());
    if(ret)
    {
        perror("detach thread error");
        exit(-1);
    }
    puts("child thread start!");
    sleep(2);
    puts("child thread end!");
    //pthread_exit((void *)0);
    return (void *)0;
}

int main(void)
{
    pthread_t tid;
    int ret = 0;
    void *tret = (void *)1;
    //printf("tret : %ld!\n", (long)tret);

    ret = pthread_create(&tid, NULL, child_thread, NULL);
    if(ret)
    {
        perror("create thread error");
        exit(-1);
    }

    sleep(1);

    ret = pthread_join(tid, &tret);
    if(ret)
        fprintf(stderr, "pthread_join error : %s!\n", strerror(ret));

    //printf("tret : %ld!\n", (long)tret);
    pthread_exit(NULL);
    //return 0;
}