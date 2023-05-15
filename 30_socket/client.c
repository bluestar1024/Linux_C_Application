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

#define MY_SERVER_IP                "192.168.43.49"
#define MY_SERVER_PORT              9999
#define MY_CLIENT_NETWORKCARD       "ens33"

int main(int argc, char *argv[])
{
    int client_fd = -1;
    struct sockaddr_in client_addr = {0}, server_addr = {0};
    struct in_addr server_ip_bin = {0};
    char client_ip_str[20] = {0};
    char buf[50] = {0};
    int ret = 0;
    const char *retp = NULL;

    struct ifreq ifr = {0};

    client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(client_fd < 0)
    {
        perror("socket error");
        exit(-1);
    }

    client_addr.sin_family = AF_INET;
    client_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    client_addr.sin_port = htons(atoi(argv[1]));
    ret = bind(client_fd, (struct sockaddr *)&client_addr, sizeof(struct sockaddr_in));
    if(ret < 0)
    {
        perror("bind error");
        exit(-1);
    }
    strcpy(ifr.ifr_name, MY_CLIENT_NETWORKCARD);
    ioctl(client_fd, SIOCGIFADDR, &ifr);
    retp = inet_ntop(AF_INET, &((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr, client_ip_str, sizeof(client_ip_str));
    if(NULL == retp)
    {
        perror("inet_ntop server error");
        exit(-1);
    }
    printf("client ip: %s\n", client_ip_str);
    printf("client port: %s\n", argv[1]);

    ret = inet_pton(AF_INET, MY_SERVER_IP, &server_ip_bin);
    if(ret != 1)
    {
        fprintf(stderr, "inet_pton error");
        exit(-1);
    }
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = server_ip_bin.s_addr;
    server_addr.sin_port = htons(MY_SERVER_PORT);
    ret = connect(client_fd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr_in));
    if(ret < 0)
    {
        perror("connect error");
        exit(-1);
    }

    while(1)
    {
        memset(buf, 0, sizeof(buf));
        fgets(buf, sizeof(buf), stdin);
        ret = send(client_fd, buf, strlen(buf), 0);
        if(ret <= 0)
        {
            perror("send error");
            exit(-1);
        }
        if(!strcmp("exit\n", buf))
            break;
    }

    close(client_fd);
    return 0;
}