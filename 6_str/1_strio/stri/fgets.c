#include <stdio.h>
#include <stdlib.h>
int main(void)
{
    char str[50] = {0};
    FILE *fp = fopen("./fgets_file_r","r+");
    if(NULL == fp)
    {
        perror("fopen error!");
        exit(0);
    }
    fgets(str,sizeof(str),fp);
    printf("%s",str);
    return 0;
}