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
#include <setjmp.h>
#include "png.h"

unsigned short *screen_base = NULL;
unsigned int width = 0;
unsigned int height = 0;
unsigned int bpp = 0;
unsigned int line_length = 0;

void show_png_image(char *path)
{
    png_structp png_ptr = NULL;
    png_infop info_ptr = NULL;
    png_bytepp row_pointes = NULL;
    unsigned short *line_buf = NULL;
    unsigned int png_width = 0, png_height = 0;
    unsigned char png_bit_depth = 0, png_color_type = 0;
    unsigned int min_w = 0, min_h = 0;
    unsigned int min_line_bytes = 0;
    unsigned int line_bytes = 0;
    FILE *image_file = NULL;
    int ret = 0;
    int i = 0, j = 0, k = 0;

    image_file = fopen(path, "r");
    if(NULL == image_file)
    {
        perror("open png file error");
        exit(-1);
    }

    png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if(NULL == png_ptr)
    {
        perror("create png struct error");
        fclose(image_file);
        exit(-1);
    }

    info_ptr = png_create_info_struct(png_ptr);
    if(NULL == info_ptr)
    {
        perror("create info struct error");
        png_destroy_read_struct(&png_ptr, NULL, NULL);
        fclose(image_file);
        exit(-1);
    }

    ret = setjmp(png_jmpbuf(png_ptr));
    if(ret)
    {
        perror("setjmp error");
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        fclose(image_file);
        exit(-1);
    }

    png_init_io(png_ptr, image_file);

    png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_STRIP_ALPHA, NULL);

    png_width = png_get_image_width(png_ptr, info_ptr);
    png_height = png_get_image_height(png_ptr, info_ptr);
    png_bit_depth = png_get_bit_depth(png_ptr, info_ptr);
    png_color_type = png_get_color_type(png_ptr, info_ptr);
    printf("png_width: %d\n"
            "png_height %d\n"
            "png_bit_depth: %d\n"
            "png_color_type: %d\n", \
            png_width, png_height, png_bit_depth, png_color_type);

    if((8 != png_bit_depth) || (PNG_COLOR_TYPE_RGB != png_color_type))
    {
        puts("Error: Not 8bit depth or not RGB color");
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        fclose(image_file);
        exit(-1);
    }
    
    if(png_width > width)
        min_w = width;
    else
        min_w = png_width;
    printf("min_w: %d\n", min_w);
    if(png_height > height)
        min_h = height;
    else
        min_h = png_height;
    printf("min_h: %d\n", min_h);

    min_line_bytes = min_w * bpp / 8;
    printf("min_line_bytes: %d\n", min_line_bytes);
    line_bytes = min_w * 3;
    printf("line_bytes: %d\n", line_bytes);
    line_buf = malloc(min_line_bytes);
    if(NULL == line_buf)
    {
        perror("malloc error");
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        fclose(image_file);
        exit(-1);
    }

    row_pointes = png_get_rows(png_ptr, info_ptr);
    if(NULL == row_pointes)
    {
        perror("png_get_rows error");
        free(line_buf);
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        fclose(image_file);
        exit(-1);
    }

    for(; i < min_h; i++)
    {
        for(j = 0, k = 0; j < line_bytes; j += 3, k++)
            line_buf[k] = ((row_pointes[i][j] & 0xf8) << 8) | \
                            ((row_pointes[i][j+1] & 0xfc) << 3) | \
                            ((row_pointes[i][j+2] & 0xf8) >> 3);
        
        memcpy(screen_base, line_buf, min_line_bytes);
        screen_base += width;
    }

    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);

    free(line_buf);
    fclose(image_file);
}

int main(int argc, char *argv[])
{
    unsigned short *temp_base = NULL;
    unsigned int screen_size = 0;
    struct fb_var_screeninfo fb_var = {0};
    struct fb_fix_screeninfo fb_fix = {0};
    int fd = -1;
    int ret = 0;

    if(2 != argc)
    {
        fprintf(stderr, "usage: %s <png file>\n", argv[0]);
        exit(-1);
    }

    fd = open("/dev/fb0", O_RDWR);
    if(fd < 0)
    {
        perror("open lcd error");
        exit(-1);
    }

    ret = ioctl(fd, FBIOGET_VSCREENINFO, &fb_var);
    if(ret < 0)
    {
        perror("ioctl var error");
        close(fd);
        exit(-1);
    }
    ret = ioctl(fd, FBIOGET_FSCREENINFO, &fb_fix);
    if(ret < 0)
    {
        perror("ioctl var error");
        close(fd);
        exit(-1);
    }

    screen_size = fb_fix.line_length * fb_var.yres;
    line_length = fb_fix.line_length;
    width = fb_var.xres;
    height = fb_var.yres;
    bpp = fb_var.bits_per_pixel;
    printf("screen_size: %d\n"
            "lcd size: %d * %d\n"
            "line_length: %d\n"
            "bpp: %d\n", \
            screen_size, width, height, line_length, bpp);
    
    screen_base = mmap(NULL, screen_size, PROT_WRITE, MAP_SHARED, fd, 0);
    if(MAP_FAILED == (void *)screen_base)
    {
        perror("mmap error");
        close(fd);
        exit(-1);
    }
    temp_base = screen_base;

    memset(screen_base, 0x0, screen_size);
    show_png_image(argv[1]);

    munmap(temp_base, screen_size);
    close(fd);
    return 0;
}