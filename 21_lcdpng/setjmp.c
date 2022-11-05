#include <stdio.h>
#include <setjmp.h>

jmp_buf buf = {0};

void my_fun(void)
{
    puts("third");
    longjmp(buf, 1);
    puts("fourth");
}

int main(void)
{
    int ret = setjmp(buf);
    if(0 == ret)
    {
        puts("first");
        my_fun();
        puts("second");
    }
    else if(1 == ret)
        puts("fifth");
    return 0;
}