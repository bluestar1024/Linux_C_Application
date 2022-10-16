#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <poll.h>
#include <linux/input.h>
#include "tslib.h"

int main(void)
{
    struct tsdev *ts = NULL;
    struct ts_sample samp = {0};
    int pressure = 0;
    int ret = 0;

    ts = ts_setup(NULL, 0);
    if(NULL == ts)
    {
        puts("ts_setup error!");
        exit(-1);
    }

    while(1)
    {
        ret = ts_read(ts, &samp, 1);
        if(ret < 0)
        {
            puts("ts_read error!");
            ts_close(ts);
            exit(-1);
        }

        if(samp.pressure > 0)
        {
            if(pressure > 0)
                printf("slide(%d, %d)\n", samp.x, samp.y);
            else
                printf("press(%d, %d)\n", samp.x, samp.y);
        }
        else
            puts("release");
        
        pressure = samp.pressure;
    }

    ts_close(ts);
    return 0;
}