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
#include <alsa/asoundlib.h>

#define PCM_CAPTURE_DEV    "hw:0,0"

snd_pcm_t *pcm = NULL;
snd_pcm_hw_params_t *cfg = NULL;

void pcm_init(void)
{
    int ret = 0;

    ret = snd_pcm_open(&pcm, PCM_CAPTURE_DEV, SND_PCM_STREAM_CAPTURE, 0);
    if(ret < 0)
    {
        fprintf(stderr, "snd_pcm_open error: %s!\n", snd_strerror(ret));
        exit(-1);
    }

    ret = snd_pcm_hw_params_malloc(&cfg);
    if(ret < 0)
    {
        fprintf(stderr, "snd_pcm_hw_params_malloc error: %s!\n", snd_strerror(ret));
        snd_pcm_close(pcm);
        exit(-1);
    }

    ret = snd_pcm_hw_params_any(pcm, cfg);
    if(ret < 0)
    {
        fprintf(stderr, "snd_pcm_hw_params_any error: %s!\n", snd_strerror(ret));
        snd_pcm_hw_params_free(cfg);
        snd_pcm_close(pcm);
        exit(-1);
    }

    ret = snd_pcm_hw_params_set_access(pcm, cfg, SND_PCM_ACCESS_RW_INTERLEAVED);
    if(ret < 0)
    {
        fprintf(stderr, "snd_pcm_hw_params_set_access error: %s!\n", snd_strerror(ret));
        snd_pcm_hw_params_free(cfg);
        snd_pcm_close(pcm);
        exit(-1);
    }

    ret = snd_pcm_hw_params_test_format(pcm, cfg, SND_PCM_FORMAT_S16_LE);
    if(ret)
    {
        fprintf(stderr, "snd_pcm_hw_params_test_format error: %s!\n", snd_strerror(ret));
        snd_pcm_hw_params_free(cfg);
        snd_pcm_close(pcm);
        exit(-1);
    }
    else
    {
        ret = snd_pcm_hw_params_set_format(pcm, cfg, SND_PCM_FORMAT_S16_LE);
        if(ret)
        {
            fprintf(stderr, "snd_pcm_hw_params_set_format error: %s!\n", snd_strerror(ret));
            snd_pcm_hw_params_free(cfg);
            snd_pcm_close(pcm);
            exit(-1);
        }
    }

    ret = snd_pcm_hw_params_set_channels(pcm, cfg, 2);
    if(ret < 0)
    {
        fprintf(stderr, "snd_pcm_hw_params_set_channels error: %s!\n", snd_strerror(ret));
        snd_pcm_hw_params_free(cfg);
        snd_pcm_close(pcm);
        exit(-1);
    }

    ret = snd_pcm_hw_params_set_rate(pcm, cfg, 44100, 0);
    if(ret < 0)
    {
        fprintf(stderr, "snd_pcm_hw_params_set_rate error: %s!\n", snd_strerror(ret));
        snd_pcm_hw_params_free(cfg);
        snd_pcm_close(pcm);
        exit(-1);
    }

    ret = snd_pcm_hw_params_set_period_size(pcm, cfg, 1024, 0);
    if(ret < 0)
    {
        fprintf(stderr, "snd_pcm_hw_params_set_period_size error: %s!\n", snd_strerror(ret));
        snd_pcm_hw_params_free(cfg);
        snd_pcm_close(pcm);
        exit(-1);
    }

    ret = snd_pcm_hw_params_set_buffer_size(pcm, cfg, 16 * 1024);
    if(ret < 0)
    {
        fprintf(stderr, "snd_pcm_hw_params_set_buffer_size error: %s!\n", snd_strerror(ret));
        snd_pcm_hw_params_free(cfg);
        snd_pcm_close(pcm);
        exit(-1);
    }

    ret = snd_pcm_hw_params(pcm, cfg);
    if(ret < 0)
    {
        fprintf(stderr, "snd_pcm_hw_params error: %s!\n", snd_strerror(ret));
        snd_pcm_hw_params_free(cfg);
        snd_pcm_close(pcm);
        exit(-1);
    }
}

int main(int argc, char *argv[])
{
    char *buf = NULL;
    unsigned int buf_bytes = 1024 * 4;
    int fd = -1;
    int ret_frame = 0;
    int ret = 0;

    if(2 != argc)
    {
        fprintf(stderr, "usage: %s <capture file>", argv[0]);
        exit(-1);
    }

    pcm_init();

    fd = open(argv[1], O_WRONLY | O_CREAT | O_EXCL, 0666);
    if(fd < 0)
    {
        perror("open capture file error");
        snd_pcm_hw_params_free(cfg);
        snd_pcm_close(pcm);
        exit(-1);
    }

    buf = malloc(buf_bytes);
    if(NULL == buf)
    {
        perror("malloc error");
        snd_pcm_hw_params_free(cfg);
        snd_pcm_close(pcm);
        close(fd);
        exit(-1);
    }

    while(1)
    {
        memset(buf, 0, buf_bytes);
        ret_frame = snd_pcm_readi(pcm, buf, 1024);
        if(ret_frame < 0)
        {
            fprintf(stderr, "read pcm data error: %s!\n", snd_strerror(ret_frame));
            free(buf);
            snd_pcm_hw_params_free(cfg);
            snd_pcm_close(pcm);
            close(fd);
            exit(-1);
        }

        ret = write(fd, buf, ret_frame * 4);
        if(ret <= 0)
        {
            perror("write data error");
            free(buf);
            snd_pcm_hw_params_free(cfg);
            snd_pcm_close(pcm);
            close(fd);
            exit(-1);
        }
    }

    free(buf);
    snd_pcm_hw_params_free(cfg);
    snd_pcm_close(pcm);
    close(fd);
    return 0;
}