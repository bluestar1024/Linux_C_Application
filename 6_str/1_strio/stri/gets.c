#include <stdio.h>
int main(void)
{
    char str[50];
    printf("please input:");
    gets(str);
    printf("%s\n",str);
    return 0;
}