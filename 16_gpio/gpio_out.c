#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

char gpio_path[100] = {0};

void gpio_write_config(char *attr, char *val)
{
    char file_path[100] = {0};
    int fd = -1;
    int len = 0;
    int ret = 0;

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

    if(3 != argc)
    {
        perror("usage error");
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

    gpio_write_config("direction", "out");
    gpio_write_config("active_low", "0");
    gpio_write_config("value", argv[2]);

    fd = open("/sys/class/gpio/unexport", O_WRONLY);
    if(fd < 0)
    {
        perror("open error");
        exit(-1);
    }
    ret = write(fd, argv[1], len);
    if(ret != len)
    {
        perror("write error");
        exit(-1);
    }
    close(fd);
    return 0;
}