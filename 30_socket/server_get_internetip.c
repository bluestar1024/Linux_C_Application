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
#include <netdb.h>

#define SERVER_PORT     9999

int main(void)
{
    int server_fd = -1, client_fd = -1;
    struct sockaddr_in server_addr = {0}, client_addr = {0};
    unsigned int len = sizeof(struct sockaddr_in);
    char client_ip_str[20] = {0}, server_ip_str[20] = {0};
    char buf[50] = {0};
    int ret = 0;
    const char *retp = NULL;

    char host_name[128] = {0};
    struct addrinfo *ailist = NULL, *aip = NULL;
    struct sockaddr_in *saddr = NULL;

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(server_fd < 0)
    {
        perror("socket error");
        exit(-1);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(SERVER_PORT);
    ret = bind(server_fd, (struct sockaddr *)&server_addr, len);
    if(ret < 0)
    {
        perror("bind error");
        exit(-1);
    }
    gethostname(host_name, sizeof(host_name));
    getaddrinfo(host_name, NULL, NULL, &ailist);
    for(aip = ailist; aip != NULL; aip = aip->ai_next)
    {
        if(AF_INET == aip->ai_family)
        {
            saddr = (struct sockaddr_in *)aip->ai_addr;
            inet_ntop(AF_INET, &saddr->sin_addr, server_ip_str, sizeof(server_ip_str));
            break;
        }
    }
    printf("server ip: %s\n", server_ip_str);
    printf("server port: %d\n", SERVER_PORT);

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
        perror("inet_ntop error");
        exit(-1);
    }
    printf("client ip: %s\n", client_ip_str);
    printf("client port: %d\n", ntohs(client_addr.sin_port));

    while(1)
    {
        memset(buf, 0, sizeof(buf));
        ret = recv(client_fd, buf, sizeof(buf), 0);
        if(ret < 0)
        {
            perror("recv error");
            exit(-1);
        }
        printf("recv buf: %s\n", buf);
        if(!strcmp("exit", buf))
            break;
    }

    close(client_fd);
    close(server_fd);
    return 0;
}