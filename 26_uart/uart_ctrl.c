#define _GNU_SOURCE

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
#include <signal.h>
#include <termios.h>

typedef struct uart_hardware_config{
    int baudrate;
    int dbit;
    int sbit;
    char parity;
}uh_cfg_t;

int fd = -1;

void uart_init(uh_cfg_t *cfg)
{
    struct termios old_cfg = {0};
    struct termios new_cfg = {0};
    int ret = 0;

    fd = open("/dev/ttymxc2", O_RDWR | O_NOCTTY);
    if(fd < 0)
    {
        perror("open uart error");
        exit(-1);
    }

    ret = tcgetattr(fd, &old_cfg);
    if(ret < 0)
    {
        perror("get uart old attr error");
        exit(-1);
    }

    memset(&new_cfg, 0x0, sizeof(struct termios));
    cfmakeraw(&new_cfg);

    new_cfg.c_cflag |= CREAD;

    switch(cfg->baudrate)
    {
        case 115200:
            ret = cfsetspeed(&new_cfg, B115200);
            break;
        case 57600:
            ret = cfsetspeed(&new_cfg, B57600);
            break;
        case 38400:
            ret = cfsetspeed(&new_cfg, B38400);
            break;
        case 19200:
            ret = cfsetspeed(&new_cfg, B19200);
            break;
        default:
            ret = cfsetspeed(&new_cfg, B115200);
            break;
    }
    if(ret < 0)
    {
        perror("baudrrate error");
        exit(-1);
    }
    else
        printf("baudrate: %d\n", cfg->baudrate);

    switch(cfg->dbit)
    {
        case 8:
            new_cfg.c_cflag &= ~CSIZE;
            new_cfg.c_cflag |= CS8;
            puts("dbit: 8");
            break;
        case 7:
            new_cfg.c_cflag &= ~CSIZE;
            new_cfg.c_cflag |= CS7;
            puts("dbit: 7");
            break;
        case 6:
            new_cfg.c_cflag &= ~CSIZE;
            new_cfg.c_cflag |= CS6;
            puts("dbit: 6");
            break;
        case 5:
            new_cfg.c_cflag &= ~CSIZE;
            new_cfg.c_cflag |= CS5;
            puts("dbit: 5");
            break;
        default:
            new_cfg.c_cflag &= ~CSIZE;
            new_cfg.c_cflag |= CS8;
            puts("dbit: 8");
            break;
    }

    switch(cfg->sbit)
    {
        case 1:
            new_cfg.c_cflag &= ~CSTOPB;
            puts("sbit: 1");
            break;
        case 2:
            new_cfg.c_cflag |= CSTOPB;
            puts("sbit: 2");
            break;
        default:
            new_cfg.c_cflag &= ~CSTOPB;
            puts("sbit: 1");
            break;
    }

    switch(cfg->parity)
    {
        case 'N':
            new_cfg.c_cflag &= ~PARENB;
            new_cfg.c_iflag &= ~INPCK;
            puts("no parity");
            break;
        case 'O':
            new_cfg.c_cflag |= PARENB | PARODD;
            new_cfg.c_iflag |= INPCK;
            puts("odd parity");
            break;
        case 'E':
            new_cfg.c_cflag |= PARENB;
            new_cfg.c_cflag &= ~PARODD;
            new_cfg.c_iflag |= INPCK;
            puts("even parity");
            break;
        default:
            new_cfg.c_cflag &= ~PARENB;
            new_cfg.c_iflag &= ~INPCK;
            puts("no parity");
            break;
    }

    new_cfg.c_cc[VMIN] = 0;
    new_cfg.c_cc[VTIME] = 0;

    tcflush(fd, TCIOFLUSH);

    ret = tcsetattr(fd, TCSANOW, &new_cfg);
    if(ret < 0)
    {
        perror("set uart new attr error");
        exit(-1);
    }
}

void uart_sig_read(int sig, siginfo_t *info, void *context)
{
    char buf[8] = {0};
    int ret = 0;
    int i = 0;

    if(POLL_IN == info->si_code)
    {
        ret = read(info->si_fd, buf, 8);
        printf("read: ");
        if(ret > 0)
        {
            for(; i < ret; i++)
                printf("%#x ", buf[i]);
        }
        puts("");
    }
}

void sig_init(void)
{
    struct sigaction sig = {0};
    int flag = 0;
    int ret = 0;

    flag = fcntl(fd, F_GETFL);
    flag |= O_ASYNC;
    fcntl(fd, F_SETFL, flag);

    fcntl(fd, F_SETOWN, getpid());
    fcntl(fd, F_SETSIG, SIGRTMIN);

    sig.sa_sigaction = uart_sig_read;
    sigemptyset(&sig.sa_mask);
    sig.sa_flags = SA_SIGINFO;
    ret = sigaction(SIGRTMIN, &sig, NULL);
    if(ret < 0)
    {
        perror("register sig error");
        exit(-1);
    }
}

int main(int argc, char *argv[])
{
    char buf[8] = {0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99};
    uh_cfg_t cfg = {0};
    int rw_flag = -1;
    int n = 0;

    for(n = 1; n < argc; n++)
    {
        if(!(strncmp("--baudrate=", argv[n], 11)))
            cfg.baudrate = atoi(&argv[n][11]);
        else if(!(strncmp("--dbit=", argv[n], 7)))
            cfg.dbit = atoi(&argv[n][7]);
        else if(!(strncmp("--sbit=", argv[n], 7)))
            cfg.sbit = atoi(&argv[n][7]);
        else if(!(strncmp("--parity=", argv[n], 9)))
            cfg.parity = argv[n][9];
        else if(!(strncmp("--type=", argv[n], 7)))
        {
            if(!(strncmp("read", &argv[n][7], 4)))
                rw_flag = 0;
            else if(!(strncmp("write", &argv[n][7], 5)))
                rw_flag = 1;
        }
    }

    if(-1 == rw_flag)
        exit(-1);

    uart_init(&cfg);

    if(0 == rw_flag)
    {
        sig_init();
        while(1)
            sleep(1);
    }
    else if(1 == rw_flag)
    {
        while(1)
        {
            write(fd, buf, 8);
            sleep(1);
        }
    }
    return 0;
}