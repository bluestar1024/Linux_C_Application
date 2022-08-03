#include <stdio.h>
#include <stdlib.h>
int main(void)
{
    FILE *fp = fopen("./fputc_file_w","w+");
    if(NULL == fp)
    {
        perror("fopen error");
        exit(-1);
    }
    fputc('a',fp);
    fputc('b',fp);
    fputc('c',fp);
    fputc('d',fp);
    fputc('\n',fp);
    return 0;
}