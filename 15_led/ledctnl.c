#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#define BRIGHTNESS          "/sys/class/leds/sys-led/brightness"
#define MAX_BRIGHTNESS      "/sys/class/leds/sys-led/max_brightness"
#define TRIGGER             "/sys/class/leds/sys-led/trigger"
#define USAGE()             fprintf(stdout, "usage :\n"                     \
                                            "    %s <on|off>\n"             \
                                            "    %s <trigger> <type>\n",    \
                                            argv[0], argv[0])

int main(int argc, char *argv[])
{
    int fd1 = -1, fd2 = -1, fd3 = -1;
    char buf[100] = {0};

    if(argc < 2)
    {
        USAGE();
        exit(-1);
    }

    fd1 = open(BRIGHTNESS, O_RDWR);
    if(fd1 < 0)
    {
        perror("open fd1 error");
        exit(-1);
    }
    fd2 = open(MAX_BRIGHTNESS, O_RDONLY);
    if(fd2 < 0)
    {
        perror("open fd2 error");
        exit(-1);
    }
    fd3 = open(TRIGGER, O_RDWR);
    if(fd3 < 0)
    {
        perror("open fd3 error");
        exit(-1);
    }

    read(fd3, buf, sizeof(buf));
    printf("trigger mode : %s", buf);
    memset(buf, 0, sizeof(buf));

    if(!strcmp(argv[1], "on"))
    {
        read(fd1, buf, sizeof(buf));
        printf("brightness before write: %s", buf);
        write(fd3, "none", 4);
        write(fd1, "1", 1);
    }
    else if(!strcmp(argv[1], "off"))
    {
        read(fd1, buf, sizeof(buf));
        printf("brightness before write: %s", buf);
        write(fd3, "none", 4);
        write(fd1, "0", 1);
    }
    else if(!strcmp(argv[1], "max_level"))
    {
        read(fd2, buf, sizeof(buf));
        printf("max brightness level: %s", buf);
    }
    else if(!strcmp(argv[1], "trigger"))
    {
        if(3 != argc)
        {
            USAGE();
            exit(-1);
        }
        write(fd3, argv[2], strlen(argv[2]));
    }
    else
    {
        USAGE();
        exit(-1);
    }
    
    close(fd1);
    close(fd2);
    close(fd3);
    return 0;
}