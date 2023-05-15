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

#define MY_SERVER_PORT              9999
#define MY_SERVER_NETWORKCARD       "eth0"

int main(void)
{
    int server_fd = -1, client_fd = -1;
    struct sockaddr_in server_addr = {0}, client_addr = {0};
    unsigned int len = sizeof(struct sockaddr_in);
    char client_ip_str[20] = {0}, server_ip_str[20] = {0};
    char buf[50] = {0};
    int ret = 0;
    const char *retp = NULL;

    struct ifreq ifr = {0};

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(server_fd < 0)
    {
        perror("socket error");
        exit(-1);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(MY_SERVER_PORT);
    ret = bind(server_fd, (struct sockaddr *)&server_addr, len);
    if(ret < 0)
    {
        perror("bind error");
        exit(-1);
    }
    strcpy(ifr.ifr_name, MY_SERVER_NETWORKCARD);
    ioctl(server_fd, SIOCGIFADDR, &ifr);
    retp = inet_ntop(AF_INET, &((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr, server_ip_str, sizeof(server_ip_str));
    if(NULL == retp)
    {
        perror("inet_ntop server error");
        exit(-1);
    }
    printf("server ip: %s\n", server_ip_str);
    printf("server port: %d\n", MY_SERVER_PORT);

    ret = listen(server_fd, 20);
    if(ret < 0)
    {
        perror("listen error");
        exit(-1);
    }

    client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &len);
    if(client_fd < 0)
    {
        perror("accept error");
        exit(-1);
    }
    retp = inet_ntop(AF_INET, &client_addr.sin_addr, client_ip_str, sizeof(client_ip_str));
    if(NULL == retp)
    {
        perror("inet_ntop client error");
        exit(-1);
    }
    printf("client ip: %s\n", client_ip_str);
    printf("client port: %d\n", ntohs(client_addr.sin_port));

    while(1)
    {
        memset(buf, 0, sizeof(buf));
        ret = recv(client_fd, buf, sizeof(buf), 0);
        if(ret <= 0)
        {
            fprintf(stderr, "recv error");
            exit(-1);
        }
        printf("recv buf: %s", buf);
        if(!strcmp("exit\n", buf))
            break;
    }

    close(client_fd);
    close(server_fd);
    return 0;
}