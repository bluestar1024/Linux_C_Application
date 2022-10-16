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
#include <sys/mman.h>

struct mt_info{
    int x;
    int y;
    int id;
    int valid;
};

int *screen_base = NULL;
int width = 0;
int height = 0;

void lcd_draw_line(int x_start, int y_start, int dir, int length, int color)
{
    int x = x_start, y = y_start;
    int end = 0;
    int temp = 0;
    int i = 0;
    
    if(x_start < 0)
        x = 0;
    if(y_start < 0)
        y = 0;

    temp = y * width + x;
    if((1 == dir) && (y_start >= 0) && (y_start <= height - 1))
    {
        end = x_start + length - 1;
        if(end >= width)
            end = width - 1;

        for(i = x; i <= end; i++, temp++)
            screen_base[temp] = color;
    }
    else if((0 == dir) && (x_start >= 0) && (x_start <= width - 1))
    {
        end = y_start + length - 1;
        if(end >= height)
            end = height - 1;
        
        for(i = y; i <= end; i++, temp += width)
            screen_base[temp] = color;
    }
}
void lcd_fill_rectangle(int x_start, int y_start, int x_end, int y_end, int color)
{
    int temp = 0;
    int i = 0, j = 0;

    if(x_start >= width)
        x_start = width - 1;
    if(y_start >= height)
        y_start = height - 1;
    if(x_end >= width)
        x_end = width - 1;
    if(y_end >= height)
        y_end = height - 1;
    
    temp = y_start * width;
    for(i = y_start; i <= y_end; i++, temp += width)
    {
        for(j = x_start; j <= x_end; j++)
            screen_base[temp + j] = color;
    }
}

int main(int argc, char *argv[])
{
    struct input_absinfo info = {0};
    struct mt_info *mt = NULL;
    struct input_event in_ev = {0};
    int ts_fd = -1;
    int max_slots = 0;
    int slot = 0;

    struct fb_var_screeninfo fb_var = {0};
    struct fb_fix_screeninfo fb_fix = {0};
    int lcd_fd = -1;
    int screen_size = 0;

    int ret = 0;
    int len = 0;
    int i = 0;

    if(2 != argc)
    {
        fprintf(stderr, "usage : %s <touch dev>", argv[0]);
        exit(-1);
    }

    ts_fd = open(argv[1], O_RDONLY);
    if(ts_fd < 0)
    {
        perror("open ts error");
        exit(-1);
    }

    lcd_fd = open("/dev/fb0", O_RDWR);
    if(lcd_fd < 0)
    {
        perror("open lcd error");
        close(ts_fd);
        exit(-1);
    }

    ret = ioctl(ts_fd, EVIOCGABS(ABS_MT_SLOT), &info);
    if(ret < 0)
    {
        perror("ioctl ts error");
        close(ts_fd);
        close(lcd_fd);
        exit(-1);
    }

    ret = ioctl(lcd_fd, FBIOGET_VSCREENINFO, &fb_var);
    if(ret < 0)
    {
        perror("ioctl lcd var error");
        close(ts_fd);
        close(lcd_fd);
        exit(-1);
    }
    ret = ioctl(lcd_fd, FBIOGET_FSCREENINFO, &fb_fix);
    if(ret < 0)
    {
        perror("ioctl lcd fix error");
        close(ts_fd);
        close(lcd_fd);
        exit(-1);
    }

    max_slots = info.maximum - info.minimum + 1;
    printf("max_slots : %d!\n", max_slots);

    screen_size = fb_fix.line_length * fb_var.yres;
    width = fb_var.xres;
    height = fb_var.yres;

    mt = calloc(max_slots, sizeof(struct mt_info));
    if(NULL == mt)
    {
        perror("calloc error");
        close(ts_fd);
        close(lcd_fd);
        exit(-1);
    }

    screen_base = mmap(NULL, screen_size, PROT_WRITE, MAP_SHARED, lcd_fd, 0);
    if(MAP_FAILED == (void *)screen_base)
    {
        perror("mmap error");
        close(ts_fd);
        close(lcd_fd);
        free(mt);
        exit(-1);
    }

    len = sizeof(struct input_event);
    memset(mt, 0x0, max_slots * sizeof(struct mt_info));
    for(; i < max_slots; i++)
        mt[i].id = -2;

    while(1)
    {
        ret = read(ts_fd, &in_ev, len);
        if(ret != len)
        {
            perror("read ts error");
            close(ts_fd);
            close(lcd_fd);
            free(mt);
            munmap(screen_base, screen_size);
            exit(-1);
        }

        switch(in_ev.type)
        {
            case EV_ABS:
                switch(in_ev.code)
                {
                    case ABS_MT_SLOT:
                        slot = in_ev.value;
                        break;
                    case ABS_MT_TRACKING_ID:
                        mt[slot].id = in_ev.value;
                        mt[slot].valid = 1;
                        break;
                    case ABS_MT_POSITION_X:
                        mt[slot].x = in_ev.value;
                        mt[slot].valid = 1;
                        break;
                    case ABS_MT_POSITION_Y:
                        mt[slot].y = in_ev.value;
                        mt[slot].valid = 1;
                        break;
                }
                break;
            case EV_SYN:
                lcd_fill_rectangle(0, 0, width -1, height - 1, 0x0);

                for(i = 0; i < max_slots; i++)
                {
                    if(mt[i].valid)
                    {
                        if(-1 != mt[i].id)
                        {
                            lcd_draw_line(mt[i].x - 99, mt[i].y, 1, 199, 0xFF00);
                            lcd_draw_line(mt[i].x, mt[i].y - 99, 0, 199, 0xFF00);
                            lcd_draw_line(mt[i].x - 50, mt[i].y - 50, 1, 25, 0xFF);
                            lcd_draw_line(mt[i].x + 25, mt[i].y - 50, 1, 25, 0xFF);
                            lcd_draw_line(mt[i].x - 50, mt[i].y - 50, 0, 25, 0xFF);
                            lcd_draw_line(mt[i].x + 50, mt[i].y - 50, 0, 25, 0xFF);
                            lcd_draw_line(mt[i].x - 50, mt[i].y + 25, 0, 25, 0xFF);
                            lcd_draw_line(mt[i].x + 50, mt[i].y + 25, 0, 25, 0xFF);
                            lcd_draw_line(mt[i].x - 50, mt[i].y + 50, 1, 25, 0xFF);
                            lcd_draw_line(mt[i].x + 25, mt[i].y + 50, 1, 25, 0xFF);
                        }
                        
                        mt[i].id = -2;
                        mt[i].valid = 0;
                    }
                }
                break; 
        }
    }

    close(ts_fd);
    close(lcd_fd);
    free(mt);
    munmap(screen_base, screen_size);
    return 0;
}