#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <poll.h>
#include <linux/input.h>

struct st_info{
    int x;
    int y;
    int down;
    int valid;
};

int main(int argc, char **argv)
{
    struct input_event in_ev = {0};
    int fd = -1;
    int len = 0;
    int ret = 0;
    struct st_info st;

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

    st.x = 0;
    st.y = 0;
    st.down = -1;
    st.valid = 0;
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
                if(BTN_TOUCH == in_ev.code)
                {
                    st.down = in_ev.value;
                    st.valid = 1;
                }
                break;
            case EV_ABS:
                switch(in_ev.code)
                {
                    case ABS_X:
                        st.x = in_ev.value;
                        st.valid = 1;
                        break;
                    case ABS_Y:
                        st.y = in_ev.value;
                        st.valid = 1;
                        break;
                }
                break;
            case EV_SYN:
                if(SYN_REPORT == in_ev.code)
                {
                    if(st.valid)
                    {
                        switch(st.down)
                        {
                            case 1:
                                printf("press(%d, %d)\n", st.x, st.y);
                                break;
                            case 0:
                                printf("release\n");
                                break;
                            case -1:
                                printf("slide(%d, %d)\n", st.x, st.y);
                                break;
                        }

                        st.valid = 0;
                        st.down = -1;
                    }
                }
                break;
        }
    }
    return 0;
}