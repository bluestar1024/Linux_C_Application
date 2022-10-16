#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <poll.h>

char gpio_path[100] = {0};

void gpio_write_config(char *attr, char *val)
{
    char file_path[100] = {0};
    int fd = -1;
    int ret = 0;
    int len = 0;

    sprintf(file_path, "%s/%s", gpio_path, attr);
    fd = open(file_path, O_WRONLY);
    if(fd < 0)
    {
        perror("open error");
        exit(-1);
    }
    len = strlen(val);
    ret = write(fd, val, len);
    if(ret != len)
    {
        perror("write error");
        exit(-1);
    }
    close(fd);
}

int main(int argc, char *argv[])
{
    int ret = 0;
    int fd = -1;
    int len = 0;
    struct pollfd pfd = {0};
    char file_path[100] = {0};
    char val = 0;

    if(2 != argc)
    {
        fprintf(stderr, "Usage : %s <gpio>!\n", argv[0]);
        exit(-1);
    }

    sprintf(gpio_path, "/sys/class/gpio/gpio%s", argv[1]);
    ret = access(gpio_path, F_OK);
    if(ret)
    {
        fd = open("/sys/class/gpio/export", O_WRONLY);
        if(fd < 0)
        {
            perror("open error");
            exit(-1);
        }
        len = strlen(argv[1]);
        ret = write(fd, argv[1], len);
        if(ret != len)
        {
            perror("write error");
            exit(-1);
        }
        close(fd);
    }

    gpio_write_config("direction", "in");
    gpio_write_config("active_low", "0");
    gpio_write_config("edge", "both");

    sprintf(file_path, "%s/%s", gpio_path, "value");
    pfd.fd = open(file_path, O_RDONLY);
    if(pfd.fd < 0)
    {
        perror("open error");
        exit(-1);
    }

    pfd.events = POLLPRI;
    read(pfd.fd, &val, 1);

    while(1)
    {
        ret = poll(&pfd, 1, -1);
        if(ret < 0)
        {
            perror("poll error");
            exit(-1);
        }
        if(0 == ret)
        {
            puts("poll time out!");
            continue;
        }
        if(pfd.revents & POLLPRI)
        {
            ret = lseek(pfd.fd, 0, SEEK_SET);
            if(ret < 0)
            {
                perror("lseek error");
                exit(-1);
            }
            ret = read(pfd.fd, &val, 1);
            if(1 != ret)
            {
                perror("read error");
                exit(-1);
            }
            printf("gpio interrupt trigger, <value=%c>!\n", val);
        }
    }
    return 0;
}