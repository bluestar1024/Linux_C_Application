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
#include <math.h>
#include <wchar.h>
#include "ft2build.h"
#include FT_FREETYPE_H

#define argb8888_to_rgb565(color)       ({unsigned int temp = (color); \
                                        ((temp & 0xf80000) >> 8) |   \
                                        ((temp & 0xfc00) >> 5) |  \
                                        ((temp & 0xf8) >> 3);})

unsigned short *screen_base = NULL;
unsigned int width = 0;
unsigned int height = 0;

FT_Library library = NULL;
FT_Face face = NULL;

void freetype_init_char(char *font, int angle)
{
    FT_Error error = 0;
    FT_Matrix matrix = {0};
    FT_Vector pen = {0};
    float rad = 0;

    error = FT_Init_FreeType(&library);
    if(error)
    {
        puts("FT_Init_FreeType error!");
        exit(-1);
    }

    error = FT_New_Face(library, font, 0, &face);
    if(error)
    {
        puts("FT_New_Face error!");
        exit(-1);
    }

    /* 原点坐标 */
    pen.x = 0 * 64;
    pen.y = 0 * 64;     //原点设置为(0, 0)

    /* 2x2矩阵初始化 */
    rad = (1.0 * angle / 180) * M_PI;   //（角度转换为弧度）M_PI是圆周率
#if 1       //非水平方向
    matrix.xx = (FT_Fixed)( cos(rad) * 0x10000L);
    matrix.xy = (FT_Fixed)(-sin(rad) * 0x10000L);
    matrix.yx = (FT_Fixed)( sin(rad) * 0x10000L);
    matrix.yy = (FT_Fixed)( cos(rad) * 0x10000L);
#endif

#if 0       //斜体  水平方向显示的
    matrix.xx = (FT_Fixed)( cos(rad) * 0x10000L);
    matrix.xy = (FT_Fixed)( sin(rad) * 0x10000L);
    matrix.yx = (FT_Fixed)( 0 * 0x10000L);
    matrix.yy = (FT_Fixed)( 1 * 0x10000L);
#endif

    FT_Set_Transform(face, &matrix, &pen);
    error = FT_Set_Pixel_Sizes(face, 50, 0);
    if(error)
    {
        puts("FT_Set_Pixel_Sizes error!");
        exit(-1);
    }
}
void freetype_show_char(int x, int y, wchar_t *str, unsigned int color)
{
    unsigned short rgb565_color = argb8888_to_rgb565(color);
    FT_GlyphSlot slot = face->glyph;
    FT_Error error = 0;
    int len = wcslen(str);
    int start_x = 0, start_y = 0, end_x = 0, end_y = 0;
    int p = 0, q = 0;
    int i = 0, j = 0, n = 0;

    for(; n < len; n++)
    {
        error = FT_Load_Char(face, str[n], FT_LOAD_RENDER);
        if(error)
        {
            puts("FT_Load_Char error!");
            continue;
        }

        start_y = y - slot->bitmap_top;
        if(start_y < 0)
        {
            p = -start_y;
            i = 0;
        }
        else
        {
            p = 0;
            i = start_y;
        }
        end_y = start_y + slot->bitmap.rows;
        if(end_y > (int)height)
            end_y = height;

        //printf("start_y: %d, y: %d, end_y: %d\n", start_y, y, end_y);
        //printf("start_x: %d, x: %d, end_x: %d\n", start_x, x, end_x);
        //printf("slot->bitmap_top: %d, slot->bitmap.rows: %d\n",slot->bitmap_top, slot->bitmap.rows);
        //printf("slot->bitmap_left: %d, slot->bitmap.width: %d\n",slot->bitmap_left, slot->bitmap.width);
        //printf("x: %d, y: %d\n", slot->advance.x, slot->advance.y);
        for(; i < end_y; i++, p++)
        {
            start_x = x + slot->bitmap_left;
            if(start_x < 0)
            {
                q = -start_x;
                j = 0;
            }
            else
            {
                q = 0;
                j = start_x;
            }
            end_x = start_x + slot->bitmap.width;
            if(end_x > (int)width)
                end_x = width;
            
            for(; j < end_x; j++, q++)
            {
                if(slot->bitmap.buffer[p * slot->bitmap.width + q])
                    screen_base[i * width + j] = rgb565_color;
            }
        }

        x += slot->advance.x / 64;
        y += slot->advance.y / 64;
    }
}

int main(int argc, char *argv[])
{
    unsigned int screen_size = 0;
    struct fb_var_screeninfo fb_var = {0};
    struct fb_fix_screeninfo fb_fix = {0};
    int fd = -1;
    int ret = 0;

    if(3 != argc)
    {
        fprintf(stderr, "usage: <%s> <font> <angle>!\n", argv[0]);
        exit(-1);
    }

    fd = open("/dev/fb0", O_RDWR);
    if(fd < 0)
    {
        perror("open error");
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
        perror("ioctl fix error");
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

    freetype_init_char(argv[1], atoi(argv[2])); 
    memset(screen_base, 0xff, screen_size);
    
    //freetype_show_char(-77, 14, L"不识庐山", 0xff00);
    freetype_show_char(50, 100, L"不识庐山真面目，只缘身在此山中", 0xff);
    freetype_show_char(50, 200, L"无人与我立黄昏，无人问我粥可温", 0x00);

    FT_Done_Face(face);
    FT_Done_FreeType(library);

    munmap(screen_base, screen_size);
    close(fd);
    return 0;
}