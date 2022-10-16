#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <poll.h>
#include <linux/input.h>

int main(int argc, char **argv)
{
    struct input_absinfo info = {0};
    int fd = -1;
    int max_slots = 0;
    int ret = 0;

    if(2 != argc)
    {
        fprintf(stderr, "usage : %s <touch dev>!\n", argv[0]);
        exit(-1);
    }

    fd = open(argv[1], O_RDONLY);
    if(fd < 0)
    {
        perror("open error");
        exit(-1);
    }

    ret = ioctl(fd, EVIOCGABS(ABS_MT_SLOT), &info);
    if(ret < 0)
    {
        perror("ioctl error");
        close(fd);
        exit(-1);
    }

    max_slots = info.maximum - info.minimum + 1;
    printf("max_slots : %d!\n", max_slots);

    close(fd);
    return 0;
}