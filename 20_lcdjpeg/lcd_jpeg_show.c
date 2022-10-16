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
#include "jpeglib.h"

typedef struct bgr888_color{
    unsigned char red;
    unsigned char green;
    unsigned char blue;
}__attribute__ ((packed)) bgr888_t;

unsigned short *screen_base = NULL;
unsigned int width = 0;
unsigned int height = 0;
unsigned int line_length = 0;
unsigned int bpp = 0;

void show_jpeg_image(char *path)
{
    struct jpeg_error_mgr jerr = {0};
    struct jpeg_decompress_struct jinfo = {0};
    unsigned int min_width = 0;
    unsigned int min_height = 0;
    unsigned int min_line_bytes = 0;
    bgr888_t *bgr888_line_buf = NULL;
    unsigned short *rgb565_line_buf = NULL;
    FILE *image_file = NULL;
    int i = 0;

    image_file = fopen(path, "r");
    if(NULL == image_file)
    {
        perror("fopen error");
        exit(-1);
    }

    jinfo.err = jpeg_std_error(&jerr);

    jpeg_create_decompress(&jinfo);

    jpeg_stdio_src(&jinfo, image_file);

    jpeg_read_header(&jinfo, TRUE);
    printf("image_width: %d\n"
            "image_height: %d\n"
            "num_components: %d\n", \
            jinfo.image_width, jinfo.image_height, jinfo.num_components);
    
    //jinfo.out_color_space = JCS_RGB;
    //jinfo.scale_num = 1;
    //jinfo.scale_denom = 4;

    jpeg_start_decompress(&jinfo);
    printf("output_width: %d\n"
            "output_height: %d\n"
            "output_components: %d\n", \
            jinfo.output_width, jinfo.output_height, jinfo.output_components);

    if(jinfo.output_width > width)
        min_width = width;
    else
        min_width = jinfo.output_width;
    printf("min_width: %d\n", min_width);
    if(jinfo.output_height > height)
        min_height = height;
    else
        min_height = jinfo.output_height;
    printf("min_height: %d\n", min_height);
    
    min_line_bytes = min_width * bpp / 8;
    printf("min_line_bytes: %d\n", min_line_bytes);

    bgr888_line_buf = malloc(jinfo.output_width * jinfo.output_components);
    if(NULL == bgr888_line_buf)
    {
        perror("malloc bgr888_line_buf error");
        exit(-1);
    }
    rgb565_line_buf = malloc(jinfo.output_width * bpp / 8);
    if(NULL == rgb565_line_buf)
    {
        perror("malloc rgb565_line_buf error");
        exit(-1);
    }

    while(jinfo.output_scanline < min_height)
    {
        jpeg_read_scanlines(&jinfo, (unsigned char **)&bgr888_line_buf, 1);

        for(i = 0; i < jinfo.output_width; i++)
            rgb565_line_buf[i] = ((bgr888_line_buf[i].red & 0xf8) << 8) | \
                                    ((bgr888_line_buf[i].green & 0xfc) << 3) | \
                                    ((bgr888_line_buf[i].blue & 0xf8) >> 3);
        
        memcpy(screen_base, rgb565_line_buf, min_line_bytes);
        screen_base += width;
    }
    jinfo.output_scanline = jinfo.output_height;

    jpeg_finish_decompress(&jinfo);

    jpeg_destroy_decompress(&jinfo);

    free(bgr888_line_buf);
    free(rgb565_line_buf);
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
        fprintf(stderr, "usage: %s <jpeg file>\n", argv[0]);
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
        perror("ioctl fb_var error");
        exit(-1);
    }
    ioctl(fd, FBIOGET_FSCREENINFO, &fb_fix);
    if(ret < 0)
    {
        perror("ioctl fb_fix error");
        exit(-1);
    }

    screen_size = fb_fix.line_length * fb_var.yres;
    width = fb_var.xres;
    height = fb_var.yres;
    line_length = fb_fix.line_length;
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
        exit(-1);
    }
    temp_base = screen_base;

    memset(screen_base, 0x00, screen_size);
    show_jpeg_image(argv[1]);

    ret = munmap(temp_base, screen_size);
    close(fd);
    return 0;
}