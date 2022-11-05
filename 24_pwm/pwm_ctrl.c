#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <poll.h>
#include <linux/input.h>
#include <linux/fb.h>
#include <sys/mman.h>

char pwm_path[100] = {0};

void pwm_config(char *attr, char *val)
{
    char file_path[100] = {0};
    int fd = -1;
    int len = 0;
    int ret = 0;

    sprintf(file_path, "%s/%s", pwm_path, attr);

    fd = open(file_path, O_RDWR);
    if(fd < 0)
    {
        perror("open attr error");
        exit(-1);
    }

    len = strlen(val);
    ret = write(fd, val, len);
    if(ret != len)
    {
        perror("write attr error");
        close(fd);
        exit(-1);
    }

    close(fd);
}

int main(int argc, char **argv)
{
    char export_path[100] = {0};
    char unexport_path[100] = {0};
    int fd = -1;
    int ret = 0;
    if(4 != argc)
    {
        fprintf(stderr, "Usage: <%s> <id> <period> <duty_cycle>\n", argv[0]);
        exit(-1);
    }
    else
        printf("<%s> <%s> <%s> <%s>\n", argv[0], argv[1], argv[2], argv[3]);
    
    sprintf(pwm_path, "/sys/class/pwm/pwmchip%s/pwm0", argv[1]);

    ret = access(pwm_path, F_OK);
    if(ret)
    {
        sprintf(export_path, "/sys/class/pwm/pwmchip%s/export", argv[1]);
        fd = open(export_path, O_WRONLY);
        if(fd < 0)
        {
            perror("open export error");
            exit(-1);
        }

        ret = write(fd, "0", 1);
        if(ret != 1)
        {
            perror("write export error");
            close(fd);
            exit(-1);
        }

        close(fd);
    }

    pwm_config("period", argv[2]);
    pwm_config("duty_cycle", argv[3]);
    pwm_config("enable", "1");

    sprintf(unexport_path, "/sys/class/pwm/pwmchip%s/unexport", argv[1]);
    fd = open(unexport_path, O_WRONLY);
    if(fd < 0)
    {
        perror("open unexport error");
        exit(-1);
    }

    ret = write(fd, "0", 1);
    if(ret != 1)
    {
        perror("write unexport error");
        close(fd);
        exit(-1);
    }

    close(fd);
    return 0;
}