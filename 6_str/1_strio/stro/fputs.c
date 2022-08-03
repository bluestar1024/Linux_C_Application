#include <stdio.h>
#include <stdlib.h>
int main(void)
{
    char str[50] = "hello world fputs test";
    FILE *fp = fopen("./fputs_file_w","w+");
    if(NULL == fp)
    {
        perror("fopen error!");
        exit(-1);
    }
    fputs(str,fp);
    fputs(str,stdout);
    return 0;
}