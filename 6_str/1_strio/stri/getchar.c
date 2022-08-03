#include <stdio.h>
#include <stdlib.h>
int main(void)
{
    int c = 0;
    c = getchar();
    if(-1 == c)
    {
        perror("getchar error!");
        exit(-1);
    }
    printf("%c",c);
    c = getchar();
    if(-1 == c)
    {
        perror("getchar error!");
        exit(-1);
    }
    printf("%c",c);
    c = getchar();
    if(-1 == c)
    {
        perror("getchar error!");
        exit(-1);
    }
    printf("%c",c);
    c = getchar();
    if(-1 == c)
    {
        perror("getchar error!");
        exit(-1);
    }
    printf("%c",c);
    c = getchar();
    if(-1 == c)
    {
        perror("getchar error!");
        exit(-1);
    }
    printf("%c",c);
    return 0;
}