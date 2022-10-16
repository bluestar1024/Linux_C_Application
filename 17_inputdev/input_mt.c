#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <poll.h>
#include <linux/input.h>

struct mt_info{
    int x;
    int y;
    int id;
    int valid;
};

int main(int argc, char **argv)
{
    struct input_event in_ev = {0};
    struct input_absinfo info = {0};
    struct mt_info *mt = NULL;
    int max_slots = 0;
    int slot = 0;
    int fd = -1;
    int ret = 0;
    int len = 0;
    int i = 0;

    if(2 != argc)
    {
        fprintf(stderr, "Usage : %s <touch dev>", argv[0]);
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

    mt = calloc(max_slots, sizeof(struct mt_info));
    if(NULL == mt)
    {
        perror("calloc error");
        close(fd);
        exit(-1);
    }

    len = sizeof(struct input_event);
    memset(mt, 0x0, max_slots * sizeof(struct mt_info));
    for(i = 0; i < max_slots; i++)
        mt[i].id = -2;

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
            case EV_ABS:
                switch(in_ev.code)
                {
                    case ABS_MT_SLOT:
                        slot = in_ev.value;
                        break;
                    case ABS_MT_TRACKING_ID:
                        mt[slot].id = in_ev.value;
                        mt[slot].valid = 1;
                        break;
                    case ABS_MT_POSITION_X:
                        mt[slot].x = in_ev.value;
                        mt[slot].valid = 1;
                        break;
                    case ABS_MT_POSITION_Y:
                        mt[slot].y = in_ev.value;
                        mt[slot].valid = 1;
                        break;
                }
                break;
            case EV_SYN:
                for(i = 0; i < max_slots; i++)
                {
                    if(mt[i].valid)
                    {
                        if(mt[i].id >= 0)
                            printf("slot<%d>, press(%d, %d)\n", i, mt[i].x, mt[i].y);
                        else if(-1 == mt[i].id)
                            printf("slot<%d>, release\n", i);
                        else
                            printf("slot<%d>, slide(%d, %d)\n", i, mt[i].x, mt[i].y);
                        
                        mt[i].id = -2;
                        mt[i].valid = 0;
                    }
                }
                break;
        }
    }

    close(fd);
    free(mt);
    return 0;
}