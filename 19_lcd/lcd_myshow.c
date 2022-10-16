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

int *screen_base = NULL;
int width = 0;
int height = 0;

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

void five_point_color_star(void)
{
    int point = 0;
    int i = 0;

    for(point = height / 6 * width + width / 2; i < 17; i++, point += 3 * width - 1)
        screen_base[point] = 0xFF0000;
    point -= 3 * width - 1;
    for(point -= 48, i = 0; i < 16; i++, point += 3)
        screen_base[point] = 0xFFFF00;
    point -= 48;
    for(point += 4 + 3 * width, i = 0; i < 10; i++, point += 4 + 3 * width)
        screen_base[point] = 0xFF00;
    point -= 4 + 3 * width;
    for(point += 3 * width - 1, i = 0; i < 16; i++, point += 3 * width - 1)
        screen_base[point] = 0xFF;
    point -= 3 * width - 1;
    for(point += 4 - 3 * width, i = 0; i < 10; i++, point += 4 - 3 * width)
        screen_base[point] = 0xFF00FF;
    point -= 4 - 3 * width;
    for(point += 4 + 3 * width, i = 0; i < 10; i++, point += 4 + 3 * width)
        screen_base[point] = 0xFF00FF;
    point -= 4 + 3 * width;
    for(point -= 3 * width + 1, i = 0; i < 16; i++, point -= 3 * width + 1)
        screen_base[point] = 0xFF;
    point += 3 * width + 1;
    for(point += 4 - 3 * width, i = 0; i < 10; i++, point += 4 - 3 * width)
        screen_base[point] = 0xFF00;
    point -= 4 - 3 * width;
    for(point -= 48, i = 0; i < 16; i++, point += 3)
        screen_base[point] = 0xFFFF00;
    point -= 48;
    for(point -= 3 * width + 1, i = 0; i < 15; i++, point -= 3 * width + 1)
        screen_base[point] = 0xFF0000;
}

int main(void)
{
    struct fb_var_screeninfo fb_var = {0};
    struct fb_fix_screeninfo fb_fix = {0};
    int screen_size = 0;
    int fd = -1;
    int ret = 0;

    fd = open("/dev/fb0", O_RDWR);
    if(fd < 0)
    {
        perror("open error");
        exit(-1);
    }

    ret = ioctl(fd, FBIOGET_VSCREENINFO, &fb_var);
    if(ret < 0)
    {
        perror("ioctl error");
        close(fd);
        exit(-1);
    }
    ret = ioctl(fd, FBIOGET_FSCREENINFO, &fb_fix);
    if(ret < 0)
    {
        perror("ioctl error");
        close(fd);
        exit(-1);
    }

    screen_size = fb_fix.line_length * fb_var.yres;
    width = fb_var.xres;
    height = fb_var.yres;
    
    screen_base = mmap(NULL, screen_size, PROT_WRITE, MAP_SHARED, fd, 0);
    if(MAP_FAILED == (void *)screen_base)
    {
        perror("mmap error");
        close(fd);
        exit(-1);
    }

    /** clear screen **/
    lcd_fill_rectangle(0, 0, width -1, height - 1, 0x0);

    /** draw five point color star **/
    five_point_color_star();

    munmap(screen_base, screen_size);
    close(fd);
    return 0;
}