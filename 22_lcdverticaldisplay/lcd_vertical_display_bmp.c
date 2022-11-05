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

unsigned short *screen_base = NULL;
unsigned int width = 0;
unsigned int height = 0;

typedef struct {
    short type;
    int size;
    short reserved1;
    short reserved2;
    int offset;
}__attribute__ ((packed)) bmp_file_header;

typedef struct {
    int size;
    int width;
    int height;
    short planes;
    short bpp;
    int compression;
    int image_size;
    int x_pels_per_meter;
    int y_pels_per_meter;
    int clr_used;
    int clr_important;
}__attribute__ ((packed)) bmp_info_header;

void show_bmp_image(char *path)
{
    bmp_file_header file_h;
    bmp_info_header info_h;
    unsigned int line_bytes = 0;
    unsigned int min_width = 0;
    unsigned int min_height = 0;
    unsigned int max_height = height - 1;
    int fd = -1;
    int ret = 0;
    int i = 0, j = 0;

    fd = open(path, O_RDONLY);
    if(fd < 0)
    {
        perror("open bmp image error");
        exit(-1);
    }

    ret = read(fd, &file_h, sizeof(bmp_file_header));
    if(ret != sizeof(bmp_file_header))
    {
        perror("read bmp_file_header error");
        close(fd);
        exit(-1);
    }

    ret = memcmp(&file_h.type, "BM", 2);
    if(ret)
    {
        perror("no bmp");
        close(fd);
        exit(-1);
    }

    ret = read(fd, &info_h, sizeof(bmp_info_header));
    if(ret != sizeof(bmp_info_header))
    {
        perror("read bmp_info_header error");
        close(fd);
        exit(-1);
    }

    printf("file size: %d\n"
            "image offset: %d\n"
            "info header size: %d\n"
            "image size: %d * %d\n"
            "image bpp: %d\n", \
            file_h.size, file_h.offset, info_h.size, info_h.width, info_h.height, info_h.bpp);

    ret = lseek(fd, file_h.offset, SEEK_SET);
    if(ret < 0)
    {
        perror("lseek bmp start error");
        close(fd);
        exit(-1);
    }

    line_bytes = info_h.width * info_h.bpp / 8;
    printf("line_bytes: %d\n", line_bytes);
    
    unsigned short (*screen_temp)[info_h.width] = malloc(line_bytes * info_h.height);
    if(NULL == screen_temp)
    {
        perror("malloc error");
        close(fd);
        exit(-1);
    }

    if(info_h.height > 0)
    {
        for(i = info_h.height - 1; i >= 0; i--)
            read(fd, screen_temp[i], line_bytes);
        puts("route 1!");
    }
    else
    {
        info_h.height = -info_h.height;
        for(i = 0; i < info_h.height; i++)
            read(fd, screen_temp[i], line_bytes);
        puts("route 2!");
    }

    if(info_h.height > width)
        min_width = width;
    else
        min_width = info_h.height;
    printf("min_width: %d\n", min_width);
    if(info_h.width > height)
        min_height = height;
    else
        min_height = info_h.width;
    printf("min_height: %d\n", min_height);

    for(i = 0; i < min_width; i++)
        for(j = 0; j < min_height; j++)
            screen_base[(max_height - j) * width + i] = screen_temp[i][j];

    free(screen_temp);
    close(fd);
}

int main(int argc, char *argv[])
{
    unsigned int screen_size = 0;
    struct fb_var_screeninfo fb_var = {0};
    struct fb_fix_screeninfo fb_fix = {0};
    int fd = -1;
    int ret = 0;

    if(2 != argc)
    {
        fprintf(stderr, "usage: %s <bmp image file>\n", argv[1]);
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

    printf("screen_size: %d\n"
            "lcd: %d * %d\n", \
            screen_size, width, height);
    
    screen_base = mmap(NULL, screen_size, PROT_WRITE, MAP_SHARED, fd, 0);
    if(MAP_FAILED == (void *)screen_base)
    {
        perror("mmap error");
        close(fd);
        exit(-1);
    }

    memset(screen_base, 0x00, screen_size);
    show_bmp_image(argv[1]);

    munmap(screen_base, screen_size);
    close(fd);
    return 0;
}