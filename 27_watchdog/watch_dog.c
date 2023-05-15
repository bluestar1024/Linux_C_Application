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
#include <linux/watchdog.h>

int main(int argc, char **argv)
{
    int fd = -1;
    struct watchdog_info info = {0};
    int option = 0;
    int timeout = 0;
    int time = 0;
    int ret = 0;

    if(2 != argc)
    {
        fprintf(stderr, "usage %s <timeout>\n", argv[0]);
        exit(-1);
    }

    fd = open("/dev/watchdog", O_RDWR);
    if(fd < 0)
    {
        perror("open watchdog error");
        exit(-1);
    }

    ret = ioctl(fd, WDIOC_GETSUPPORT, &info);
    if(ret < 0)
    {
        perror("ioctl get support info error");
        close(fd);
        exit(-1);
    }
    printf("version: %u\n", info.firmware_version);
    printf("identity: %s\n", info.identity);
    if(0 == (WDIOF_KEEPALIVEPING & info.options))
        puts("no support feed watchdog");
    if(0 == (WDIOF_SETTIMEOUT & info.options))
        puts("no support set timeout");

    option = WDIOS_DISABLECARD;
    ret = ioctl(fd, WDIOC_SETOPTIONS, &option);
    if(ret < 0)
    {
        perror("ioctl close watchdog error");
        close(fd);
        exit(-1);
    }

    timeout = atoi(argv[1]);
    if(timeout < 1)
        timeout = 1;
    
    ret = ioctl(fd, WDIOC_SETTIMEOUT, &timeout);
    if(ret < 0)
    {
        perror("ioctl set timeout error");
        close(fd);
        exit(-1);
    }

    ret = ioctl(fd, WDIOC_GETTIMEOUT, &timeout);
    if(ret < 0)
    {
        perror("ioctl get timeout error");
        close(fd);
        exit(-1);
    }
    printf("timeout: %ds\n", timeout);

    option = WDIOS_ENABLECARD;
    ret = ioctl(fd, WDIOC_SETOPTIONS, &option);
    if(ret < 0)
    {
        perror("ioctl open watchdog error");
        close(fd);
        exit(-1);
    }

    time = (timeout * 1000 - 100) * 1000;
    while(1)
    {
        usleep(time);
        ret = ioctl(fd, WDIOC_KEEPALIVE, NULL);
        if(ret < 0)
        {
            perror("ioctl feed watchdog error");
            close(fd);
            exit(-1);
        }
    }
    return 0;
}