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

#define SERVER_IP       "192.168.43.49"
#define SERVER_PORT     9999

int main(int argc, char *argv[])
{
    int client_fd = -1;
    struct sockaddr_in client_addr = {0}, server_addr = {0};
    struct in_addr server_ip_bin = {0};
    char client_ip_str[20] = {0};
    char buf[50] = {0};
    int ret = 0;

    char host_name[128] =  {0};
    struct addrinfo *ailist = NULL, *aip = NULL;
    struct sockaddr_in *saddr = NULL;

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
    gethostname(host_name, sizeof(host_name));
    getaddrinfo(host_name, NULL, NULL, &ailist);
    for(aip = ailist; aip != NULL; aip = aip->ai_next)
    {
        if(AF_INET == aip->ai_family)
        {
            saddr = (struct sockaddr_in *)aip->ai_addr;
            inet_ntop(AF_INET, &saddr->sin_addr, client_ip_str, sizeof(client_ip_str));
            break;
        }
    }
    printf("client ip: %s\n", client_ip_str);
    printf("client port: %s\n", argv[1]);

    ret = inet_pton(AF_INET, SERVER_IP, &server_ip_bin);
    if(ret != 1)
    {
        fprintf(stderr, "inet_pton error");
        exit(-1);
    }
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = server_ip_bin.s_addr;
    server_addr.sin_port = htons(SERVER_PORT);
    ret = connect(client_fd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr_in));
    if(ret < 0)
    {
        perror("connect error");
        exit(-1);
    }

    while(1)
    {
        memset(buf, 0, sizeof(buf));
        ret = send(client_fd, buf, strlen(buf), 0);
        if(ret < 0)
        {
            perror("send error");
            exit(-1);
        }
        if(!strcmp("exit", buf))
            break;
    }

    close(client_fd);
    return 0;
}