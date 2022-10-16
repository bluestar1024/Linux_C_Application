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

short *screen_base = NULL;
int width = 0;
int height = 0;
int line_length = 0;

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
    short *line_buf = NULL;
    int line_bytes = 0;
    int min_height = 0;
    int min_line_bytes = 0;
    int fd = -1;
    int ret = 0;
    int i = 0;
    int temp = 0;

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
    line_buf = malloc(line_bytes);
    if(NULL == line_buf)
    {
        perror("malloc error");
        close(fd);
        exit(-1);
    }

    if(line_length > line_bytes)
        min_line_bytes = line_bytes;
    else
        min_line_bytes = line_length;
    printf("min_line_bytes: %d\n", min_line_bytes);
    
    if(info_h.height > 0)
    {
        if(info_h.height > height)
        {
            min_height = height;
            printf("min_height: %d, route1\n", min_height);
            lseek(fd, (info_h.height - height) * line_bytes, SEEK_CUR);
            if(ret < 0)
            {
                perror("lseek bmp middle error");
                close(fd);
                exit(-1);
            }
            screen_base += (height - 1) * width;
        }
        else
        {
            min_height = info_h.height;
            printf("min_height: %d, route2\n", min_height);
            screen_base += (info_h.height - 1) * width;
        }

        for(i = 0; i < min_height; i++, screen_base -= width)
        {
            read(fd, line_buf, line_bytes);
            memcpy(screen_base, line_buf, min_line_bytes);
        }
    }
    else
    {
        temp = -info_h.height;
        if(temp > height)
            min_height = height;
        else
            min_height = temp;
        printf("min_height: %d, route3\n", min_height);
        
        for(i = 0; i < min_height; i++, screen_base += width)
        {
            read(fd, line_buf, line_bytes);
            memcpy(screen_base, line_buf, min_line_bytes);
        }
    }

    free(line_buf);
    close(fd);
}

int main(int argc, char *argv[])
{
    int screen_size = 0;
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
    line_length = fb_fix.line_length;

    printf("fb_fix.line_length: %d\n"
            "xres: %d\n"
            "yres: %d\n"
            "screen_size: %d\n"
            "line_length: %d\n", \
            fb_fix.line_length, fb_var.xres, fb_var.yres, screen_size, line_length);
    
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