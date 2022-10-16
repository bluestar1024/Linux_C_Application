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

void lcd_draw_point(int x, int y, int color)
{
    if(x >= width)
        x = width - 1;
    if(y >= height)
        y = height - 1;
    
    screen_base[y * width + x] = color;
}

void lcd_draw_line(int x, int y, int dir, int length, int color)
{
    int end = 0;
    int temp = 0;
    int i = 0;
    
    if(x >= width)
        x = width - 1;
    if(y >= height)
        y = height - 1;

    temp = y * width + x;
    if(dir)
    {
        end = x + length - 1;
        if(end >= width)
            end = width - 1;

        for(i = x; i <= end; i++, temp++)
            screen_base[temp] = color;
    }
    else
    {
        end = y + length - 1;
        if(end >= height)
            end = height - 1;
        
        for(i = y; i <= end; i++, temp += width)
            screen_base[temp] = color;
    }
}

void lcd_draw_rectangle(int x_start, int y_start, int x_end, int y_end, int color)
{
    if(x_start >= width)
        x_start = width - 1;
    if(y_start >= height)
        y_start = height - 1;
    if(x_end >= width)
        x_end = width - 1;
    if(y_end >= height)
        y_end = height - 1;
    
    lcd_draw_line(x_start, y_start, 1, x_end - x_start + 1, color);
    lcd_draw_line(x_start, y_end, 1, x_end - x_start + 1, color);
    lcd_draw_line(x_start, y_start + 1, 0, y_end - y_start - 1, color);
    lcd_draw_line(x_end, y_start + 1, 0, y_end - y_start - 1, color);
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

int main(void)
{
    struct fb_var_screeninfo fb_var = {0};
    struct fb_fix_screeninfo fb_fix = {0};
    int screen_size = 0;
    int fd = -1;
    int ret = 0;
    int x_start = 0, y_start = 0, x_end = 0, y_end = 0;

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

    /** rectangle block **/
    lcd_fill_rectangle(0, 0, height / 4 - 1, height / 4 - 1, 0xFF0000);
    lcd_fill_rectangle(width - height / 4, 0, width - 1, height / 4 - 1, 0xFF00);
    lcd_fill_rectangle(0, height / 4 * 3, height / 4 - 1, height - 1, 0xFF);
    lcd_fill_rectangle(width - height / 4, height / 4 * 3, width - 1, height - 1, 0xFFFF00);

    /** vertical cross **/
    lcd_draw_line(0, height / 2, 1, width, 0xFFFFFF);
    lcd_draw_line(width / 2, 0, 0, height, 0xFFFFFF);

    /** rectangle **/
    x_start = width / 4;
    y_start = height / 4;
    x_end = width / 4 * 3;
    y_end = height / 4 * 3;
    while((x_start < width / 2) && (y_start < height / 2))
    {
        lcd_draw_rectangle(x_start, y_start, x_end, y_end, 0xFFFFFF);
        x_start += 5;
        y_start += 5;
        x_end -= 5;
        y_end -= 5;
    }

    munmap(screen_base, screen_size);
    close(fd);
    return 0;
}