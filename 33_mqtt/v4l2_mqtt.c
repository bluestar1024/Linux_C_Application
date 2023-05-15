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
#include <linux/videodev2.h>
#include <signal.h>

#define FRAME_BUF_COUNT   3

unsigned short *screen_base = NULL;
unsigned int screen_size = 0;
unsigned int width = 0;
unsigned int height = 0;
void *frm_base[FRAME_BUF_COUNT] = {0};
struct v4l2_buffer buf = {0};
struct v4l2_format format = {0};
enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
int v4l2_fd = -1;
int lcd_fd = -1;

void v4l2_config_init(void)
{
    struct v4l2_capability cap = {0};
    struct v4l2_fmtdesc fmt = {0};
    struct v4l2_frmsizeenum frmsize = {0};
    struct v4l2_frmivalenum frmival = {0};
    struct v4l2_streamparm sparm = {0};
    struct v4l2_requestbuffers reqbuf = {0};

    v4l2_fd = open("/dev/video1", O_RDWR);
    if(v4l2_fd < 0)
    {
        perror("open v4l2 error");
        exit(-1);
    }

    ioctl(v4l2_fd, VIDIOC_QUERYCAP, &cap);
    if(!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE))
    {
        fprintf(stderr, "no camera!\n");
        close(v4l2_fd);
        exit(-1);
    }

    fmt.index = 0;
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    while(0 == ioctl(v4l2_fd, VIDIOC_ENUM_FMT, &fmt))
    {
        printf("support: pixelformat<%#x>, description<%s>, index<%d>\n", fmt.pixelformat, fmt.description, fmt.index);
        fmt.index++;
    }

    frmsize.index = 0;
    frmsize.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    frmsize.pixel_format = V4L2_PIX_FMT_RGB565;
    while(0 == ioctl(v4l2_fd, VIDIOC_ENUM_FRAMESIZES, &frmsize))
    {
        printf("support: frame size<%d*%d>, index<%d>\n", frmsize.discrete.width, frmsize.discrete.height, frmsize.index);
        frmsize.index++;
    }

    frmival.index = 0;
    frmival.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    frmival.pixel_format = V4L2_PIX_FMT_RGB565;
    frmival.width = width;
    frmival.height = height;
    while(0 == ioctl(v4l2_fd, VIDIOC_ENUM_FRAMEINTERVALS, &frmival))
    {
        printf("support: numerator<%d>, denominator<%d>, index<%d>\n", frmival.discrete.numerator, frmival.discrete.denominator, frmival.index);
        frmival.index++;
    }

    format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    ioctl(v4l2_fd, VIDIOC_G_FMT, &format);
    printf("before setting: pixelformat<%d>, width<%d>, height<%d>\n", format.fmt.pix.pixelformat, format.fmt.pix.width, format.fmt.pix.height);

    format.fmt.pix.pixelformat = V4L2_PIX_FMT_RGB565;
    format.fmt.pix.width = width;
    format.fmt.pix.height = height;
    ioctl(v4l2_fd, VIDIOC_S_FMT, &format);
    printf("after setting: pixelformat<%d>, width<%d>, height<%d>\n", format.fmt.pix.pixelformat, format.fmt.pix.width, format.fmt.pix.height);

    if(V4L2_PIX_FMT_RGB565 != format.fmt.pix.pixelformat)
    {
        fprintf(stderr, "pixelformat modified");
        close(v4l2_fd);
        exit(-1);
    }
    if((width != format.fmt.pix.width) || (height != format.fmt.pix.height))
    {
        fprintf(stderr, "picture size modified");
        close(v4l2_fd);
        exit(-1);
    }

    sparm.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    ioctl(v4l2_fd, VIDIOC_G_PARM, &sparm);
    printf("before setting: numerator<%d>, denominator<%d>\n", sparm.parm.capture.timeperframe.numerator, sparm.parm.capture.timeperframe.denominator);
    if(sparm.parm.capture.capability & V4L2_CAP_TIMEPERFRAME)
    {
        sparm.parm.capture.timeperframe.numerator = 1;
        sparm.parm.capture.timeperframe.denominator = 30;
        ioctl(v4l2_fd, VIDIOC_S_PARM, &sparm);
        ioctl(v4l2_fd, VIDIOC_G_PARM, &sparm);
        printf("after setting: numerator<%d>, denominator<%d>\n", sparm.parm.capture.timeperframe.numerator, sparm.parm.capture.timeperframe.denominator);
    }
    else
        puts("cannot set frame rate\n");

    reqbuf.count = FRAME_BUF_COUNT;
    reqbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    reqbuf.memory = V4L2_MEMORY_MMAP;
    ioctl(v4l2_fd, VIDIOC_REQBUFS, &reqbuf);

    buf.index = 0;
    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;
    while(0 == ioctl(v4l2_fd, VIDIOC_QUERYBUF, &buf))
    {
        frm_base[buf.index] = mmap(NULL, buf.length, PROT_READ, MAP_SHARED, v4l2_fd, buf.m.offset);
        if(MAP_FAILED == frm_base[buf.index])
        {
            perror("mmap frame buf error");
            close(v4l2_fd);
            exit(-1);
        }
        buf.index++;
    }

    buf.index = 0;
    while(0 == ioctl(v4l2_fd, VIDIOC_QBUF, &buf))
        buf.index++;

    ioctl(v4l2_fd, VIDIOC_STREAMON, &type);
}
void v4l2_color_init()
{
    struct v4l2_control color_cfg = {0};
    int ret = 0;

    /**********************设置手动白平衡******************************/
    /*color_cfg.id = V4L2_CID_AUTO_WHITE_BALANCE;
    color_cfg.value = V4L2_WHITE_BALANCE_MANUAL;
    ret = ioctl(v4l2_fd, VIDIOC_S_CTRL, &color_cfg);
    if(ret < 0)
    {
        perror("设置手动白平衡错误");
        exit(-1);
    }*/

    /**********************设置白平衡色温******************************/
    /*color_cfg.id = V4L2_CID_WHITE_BALANCE_TEMPERATURE;
    color_cfg.value = 5100;
    ret = ioctl(v4l2_fd, VIDIOC_S_CTRL, &color_cfg);
    if(ret < 0)
    {
        perror("设置白平衡色温错误");
        exit(-1);
    }*/

    /**********************设置亮度******************************/
    /*color_cfg.id = V4L2_CID_BRIGHTNESS;
    color_cfg.value = 40;
    ret = ioctl(v4l2_fd, VIDIOC_S_CTRL, &color_cfg);
    if(ret < 0)
    {
        perror("设置亮度错误");
        exit(-1);
    }*/

    /**********************设置对比度******************************/
    /*color_cfg.id = V4L2_CID_CONTRAST;
    color_cfg.value = 45;
    ret = ioctl(v4l2_fd, VIDIOC_S_CTRL, &color_cfg);
    if(ret < 0)
    {
        perror("设置对比度错误");
        exit(-1);
    }*/

    /**********************设置饱和度******************************/
    /*color_cfg.id = V4L2_CID_SATURATION;
    color_cfg.value = 60;
    ret = ioctl(v4l2_fd, VIDIOC_S_CTRL, &color_cfg);
    if(ret < 0)
    {
        perror("设置饱和度错误");
        exit(-1);
    }*/

    /**********************设置色度******************************/
    /*color_cfg.id = V4L2_CID_HUE;
    color_cfg.value = 1;
    ret = ioctl(v4l2_fd, VIDIOC_S_CTRL, &color_cfg);
    if(ret < 0)
    {
        perror("设置色度错误");
        exit(-1);
    }*/

    /**********************设置锐度******************************/
    /*color_cfg.id = V4L2_CID_SHARPNESS;
    color_cfg.value = 4;
    ret = ioctl(v4l2_fd, VIDIOC_S_CTRL, &color_cfg);
    if(ret < 0)
    {
        perror("设置锐度错误");
        exit(-1);
    }*/

    /**********************设置背光补偿******************************/
    /*color_cfg.id = V4L2_CID_BACKLIGHT_COMPENSATION;
    color_cfg.value = 3;
    ret = ioctl(v4l2_fd, VIDIOC_S_CTRL, &color_cfg);
    if(ret < 0)
    {
        perror("设置背光补偿错误");
        exit(-1);
    }*/

    /**********************设置伽玛值******************************/
    /*color_cfg.id = V4L2_CID_GAMMA;
    color_cfg.value = 120;
    ret = ioctl(v4l2_fd, VIDIOC_S_CTRL, &color_cfg);
    if(ret < 0)
    {
        perror("设置伽玛值错误");
        exit(-1);
    }*/
}
void v4l2_to_lcd(void)
{
    unsigned short *temp_screen_base = NULL;
    void *temp_frm_base = NULL;
    unsigned int min_width = 0;
    unsigned int min_height = 0;
    unsigned int min_line_bytes = 0;
    int i = 0;
    int j = 0;

    if(width > format.fmt.pix.width)
        min_width = format.fmt.pix.width;
    else
        min_width = width;
    if(height > format.fmt.pix.height)
        min_height = format.fmt.pix.height;
    else
        min_height = height;
    min_line_bytes = min_width * 2;

    while(1)
    {
        for(buf.index = 0; buf.index < FRAME_BUF_COUNT; buf.index++)
        {
            ioctl(v4l2_fd, VIDIOC_DQBUF, &buf);

            //此为默认数据，默认是左右颠倒的
            /**************************************************************************************************************************************************************/
            for(i = 0, temp_screen_base = screen_base, temp_frm_base = frm_base[buf.index]; i < min_height; i++, temp_screen_base += width, temp_frm_base += min_line_bytes)
                memcpy(temp_screen_base, temp_frm_base, min_line_bytes);
            /**************************************************************************************************************************************************************/

            //此为调整数据，与原形一样
            /**************************************************************************************************************************************************************/
            /*for(i = 0, temp_screen_base = screen_base, temp_frm_base = frm_base[buf.index]; i < min_height; i++, temp_frm_base += min_line_bytes + 2)
            {
                for(j = min_width, temp_frm_base += min_line_bytes - 2; j > 0; j--, temp_screen_base ++, temp_frm_base -= 2)
                    memcpy(temp_screen_base, temp_frm_base, 2);
            }*/
            /**************************************************************************************************************************************************************/

            ioctl(v4l2_fd, VIDIOC_QBUF, &buf);
        }
    }
}
void v4l2_exit(void)
{
    ioctl(v4l2_fd, VIDIOC_STREAMOFF, &type);

    buf.index = 0;
    while(0 == ioctl(v4l2_fd, VIDIOC_QUERYBUF, &buf))
    {
        munmap(frm_base[buf.index], buf.length);
        buf.index++;
    }

    close(v4l2_fd);
}

void lcd_init(void)
{
    struct fb_var_screeninfo fb_var = {0};
    struct fb_fix_screeninfo fb_fix = {0};

    lcd_fd = open("/dev/fb0", O_RDWR);
    if(lcd_fd < 0)
    {
        perror("open lcd error");
        exit(-1);
    }

    ioctl(lcd_fd, FBIOGET_VSCREENINFO, &fb_var);
    ioctl(lcd_fd, FBIOGET_FSCREENINFO, &fb_fix);

    screen_size = fb_fix.line_length * fb_var.yres;
    width = fb_var.xres;
    height = fb_var.yres;

    screen_base = mmap(NULL, screen_size, PROT_WRITE, MAP_SHARED, lcd_fd, 0);
    if(MAP_FAILED == (void *)screen_base)
    {
        perror("mmap lcd error");
        close(lcd_fd);
        exit(-1);
    }

    memset(screen_base, 0x0, screen_size);
}
void lcd_exit(void)
{
    memset(screen_base, 0x0, screen_size);
    munmap(screen_base, screen_size);
    close(lcd_fd);
}

void v4l2_sig_handle(int sig, siginfo_t *info, void *text)
{
    v4l2_exit();
    lcd_exit();
    exit(0);
}
void v4l2_sig_init(void)
{
    struct sigaction sig = {0};
    sig.sa_sigaction = v4l2_sig_handle;
    sig.sa_flags = SA_SIGINFO;
    int ret = 0;
    ret = sigaction(SIGUSR1, &sig, NULL);
    if(ret < 0)
    {
        perror("sigaction error");
        exit(-1);
    }
}

int main(void)
{
    v4l2_sig_init();
    lcd_init();
    v4l2_config_init();
    //v4l2_color_init();    //ov5640不支持色彩设置
    v4l2_to_lcd();
    v4l2_exit();
    lcd_exit();
    return 0;
}