#include <stdio.h>
#include <stdlib.h>
int main(void)
{
    int c = 0;
    FILE *fp = fopen("./fgetc_file_r","r+");
    if(NULL == fp)
    {
        perror("fopen error!");
        exit(-1);
    }
    c = fgetc(fp);
    printf("%c",c);
    c = fgetc(fp);
    printf("%c",c);
    c = fgetc(fp);
    printf("%c",c);
    c = fgetc(fp);
    printf("%c",c);
    c = fgetc(fp);
    printf("%c",c);
    c = fgetc(fp);
    printf("%c",c);
    return 0;
}