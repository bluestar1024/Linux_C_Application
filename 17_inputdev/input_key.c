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
    struct input_event in_ev = {0};
    int fd = -1;
    int len = 0;
    int ret = 0;

    if(2 != argc)
    {
        fprintf(stderr, "usage : %s <input dev>!\n", argv[0]);
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
            exit(-1);
        }

        if(EV_KEY == in_ev.type)
        {
            switch(in_ev.value)
            {
                case 0:
                    printf("code<%d>, release!\n", in_ev.code);
                    break;
                case 1:
                    printf("code<%d>, press!\n", in_ev.code);
                    break;
                case 2:
                    printf("code<%d>, long press!\n", in_ev.code);
                    break;
            }
        }
    }
    return 0;
}