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
#include <signal.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <linux/can/raw.h>
#include <pthread.h>

#define CAN_DEV         "can0"

int can_fd = -1;
struct can_frame wframe = {0}, rframe = {0};
int framelen = sizeof(struct can_frame);

void *can_send(void *arg)
{
    int ret = 0;

    while(1)
    {
        puts("send start!");
        ret = write(can_fd, &wframe, framelen);
        if(ret != framelen)
        {
            perror("write error");
            exit(-1);
        }
        puts("send finish!");
        sleep(1);
    }
}

int main(int argc, char *argv[])
{
    struct sockaddr_can can_addr = {0};
    struct ifreq ifr = {0};
    struct can_filter rfilter[3] = {0};
    char wbuf[8] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};
    int loopback = 1;
    int recv_own_msgs = 1;
    int ret = 0;
    int i = 0;

    pthread_t pth = {0};

    if(2 != argc)
    {
        fprintf(stderr, "usage: %s <frameID to send>\n", argv[0]);
        exit(-1);
    }

    can_fd = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if(can_fd < 0)
    {
        perror("socket error");
        exit(-1);
    }

    strcpy(ifr.ifr_name, CAN_DEV);
    ioctl(can_fd, SIOCGIFINDEX, &ifr);
    can_addr.can_family = AF_CAN;
    can_addr.can_ifindex = ifr.ifr_ifindex;
    ret = bind(can_fd, (struct sockaddr *)&can_addr, sizeof(can_addr));
    if(ret < 0)
    {
        perror("bind error");
        exit(-1);
    }

    for(; i < sizeof(rfilter) / sizeof(rfilter[0]); i++)
    {
        rfilter[i].can_id = i * 16 + i + 34;
        rfilter[i].can_mask = 0x7ff;
    }
    ret = setsockopt(can_fd, SOL_CAN_RAW, CAN_RAW_FILTER, rfilter, sizeof(rfilter));
    if(ret < 0)
    {
        perror("setsockopt filter error");
        exit(-1);
    }
    ret = setsockopt(can_fd, SOL_CAN_RAW, CAN_RAW_LOOPBACK, &loopback, sizeof(loopback));
    if(ret < 0)
    {
        perror("setsockopt loopback error");
        exit(-1);
    }
    ret = setsockopt(can_fd, SOL_CAN_RAW, CAN_RAW_RECV_OWN_MSGS, &recv_own_msgs, sizeof(recv_own_msgs));
    if(ret < 0)
    {
        perror("setsockopt recv_own_msgs error");
        exit(-1);
    }
    
    wframe.can_id = atoi(argv[1]) / 10 * 16 + atoi(argv[1]) % 10;
    wframe.can_dlc = sizeof(wbuf);
    memcpy(wframe.data, wbuf, sizeof(wbuf));

    ret = pthread_create(&pth, NULL, can_send, NULL);
    if(ret < 0)
    {
        perror("pthread_create error");
        exit(-1);
    }

    while(1)
    {
        memset(&rframe, 0x00, framelen);
        ret = read(can_fd, &rframe, framelen);
        if(ret != framelen)
        {
            perror("read error");
            exit(-1);
        }
        if(rframe.can_id & CAN_ERR_FLAG)
        {
            fprintf(stderr, "can err frame\n");
            exit(-1);
        }
        if(rframe.can_id & CAN_EFF_FLAG)
            printf("can extend frameID: %#x\n", rframe.can_id & CAN_EFF_MASK);
        else
            printf("can standard frameID: %#x\n", rframe.can_id & CAN_SFF_MASK);
        if(rframe.can_id & CAN_RTR_FLAG)
        {
            puts("remote frame");
            continue;
        }
        printf("recv buf:");
        for(i = 0; i < rframe.can_dlc; i++)
            printf(" %#x", rframe.data[i]);
        puts("");
    }

    close(can_fd);
    return 0;
}