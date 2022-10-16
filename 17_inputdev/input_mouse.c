#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <poll.h>
#include <linux/input.h>

int main(int argc, char *argv[])
{
    struct input_event in_ev = {0};
    int fd = -1;
    int len = 0;
    int ret = 0;

    if(2 != argc)
    {
        fprintf(stderr, "Usage: %s <input dev>", argv[0]);
        exit(-1);
    }

    fd = open(argv[1], O_RDONLY);
    if(fd < 0)
    {
        perror("open error");
        exit(-1);
    }

    len = sizeof(struct input_event);
    while(1)
    {
        ret = read(fd, &in_ev, len);
        if(ret != len)
        {
            perror("read error");
            close(fd);
            exit(-1);
        }

        switch(in_ev.type)
        {
            case EV_KEY:
                switch(in_ev.code)
                {
                    case BTN_LEFT:
                        if(in_ev.value)
                            puts("mouse left button press");
                        else
                            puts("mouse left button release");
                        break;
                    case BTN_RIGHT:
                        if(in_ev.value)
                            puts("mouse right button press");
                        else
                            puts("mouse right button release");
                        break;
                    case BTN_MIDDLE:
                        if(in_ev.value)
                            puts("mouse middle button press");
                        else
                            puts("mouse middle button release");
                        break;
                }
                break;
            case EV_REL:
                switch(in_ev.code)
                {
                    case REL_X:
                        if(in_ev.value < 0)
                            printf("mouse move left: %d\n", -in_ev.value);
                        else if(in_ev.value > 0)
                            printf("mouse move right: %d\n", in_ev.value);
                        break;
                    case REL_Y:
                        if(in_ev.value < 0)
                            printf("mouse move forward: %d\n", -in_ev.value);
                        else if(in_ev.value > 0)
                            printf("mouse move back: %d\n", in_ev.value);
                        break;
                    case REL_WHEEL:
                        if(1 == in_ev.value)
                            puts("mouse wheel move forward");
                        else if(-1 == in_ev.value)
                            puts("mouse wheel move back");
                        break;
                }
                break;
        }
    }

    close(fd);
    return 0;
}