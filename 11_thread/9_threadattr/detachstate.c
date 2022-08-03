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
    int detachstate;
    pthread_t tid;
    int ret = 0;

    pthread_attr_init(&attr);
    pthread_attr_getdetachstate(&attr, &detachstate);
    if(PTHREAD_CREATE_JOINABLE == detachstate)
        puts("detach state : joinable!");
    else
        puts("detach state : detachable!");
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    pthread_attr_getdetachstate(&attr, &detachstate);
    if(PTHREAD_CREATE_JOINABLE == detachstate)
        puts("detach state : joinable!");
    else
        puts("detach state : detachable!");

    ret = pthread_create(&tid, &attr, child_thread, NULL);
    if(ret)
    {
        perror("create thread error");
        exit(-1);
    }

    sleep(1);

    ret = pthread_join(tid, NULL);
    if(ret)
        fprintf(stderr, "pthread_join error : %s!\n", strerror(ret));

    pthread_attr_destroy(&attr);
    return 0;
}