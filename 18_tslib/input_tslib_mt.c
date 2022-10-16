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
    struct input_absinfo info = {0};
    int max_slots = 0;
    struct ts_sample_mt *samp = NULL;
    int pressure[8] = {0};
    int ret = 0;
    int i = 0;

    ts = ts_setup(NULL, 0);
    if(NULL == ts)
    {
        perror("ts_setup error");
        exit(-1);
    }

    ret = ioctl(ts_fd(ts), EVIOCGABS(ABS_MT_SLOT), &info);
    if(ret < 0)
    {
        perror("ioctl error");
        ts_close(ts);
        exit(-1);
    }

    max_slots = info.maximum - info.minimum + 1;
    printf("max_slots : %d!\n", max_slots);

    samp = calloc(max_slots, sizeof(struct ts_sample_mt));

    while(1)
    {
        ret = ts_read_mt(ts, &samp, max_slots, 1);
        if(ret < 0)
        {
            perror("ts_read_mt error");
            ts_close(ts);
            free(samp);
            exit(-1);
        }

        for(i = 0; i < max_slots; i++)
        {
            if(samp[i].valid)
            {
                if(samp[i].pressure > 0)
                {
                    //if(pressure[i] > 0)   //i equal slot
                    if(pressure[samp[i].slot] > 0)
                        printf("slot<%d>, slide<%d, %d>\n", samp[i].slot, samp[i].x, samp[i].y);
                    else
                        printf("slot<%d>, press<%d, %d>\n", samp[i].slot, samp[i].x, samp[i].y);
                }
                else
                    printf("slot<%d>, release\n", samp[i].slot);
                
                //pressure[i] = samp[i].pressure;   //i equal slot
                pressure[samp[i].slot] = samp[i].pressure;
            }
        }
    }

    ts_close(ts);
    free(samp);
    return 0;
}