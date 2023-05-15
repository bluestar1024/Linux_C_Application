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

struct pollfd *pfds = NULL;
int count = 0;

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

void pcm_poll_init(void)
{
    int ret = 0;

    count = snd_pcm_poll_descriptors_count(pcm);
    if(count <= 0)
    {
        fprintf(stderr, "snd_pcm_poll_descriptors_count error: %s!\n", snd_strerror(count));
        exit(-1);
    }

    pfds = calloc(count, sizeof(struct pollfd));
    if(NULL == pfds)
    {
        perror("calloc pfds error");
        exit(-1);
    }

    ret = snd_pcm_poll_descriptors(pcm, pfds, count);
    if(ret < 0)
    {
        fprintf(stderr, "snd_pcm_poll_descriptors error: %s!\n", snd_strerror(ret));
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
    unsigned short revents = 0;
    unsigned int read_frame = 0;

    if(2 != argc)
    {
        fprintf(stderr, "usage: %s <capture file>", argv[0]);
        exit(-1);
    }

    pcm_init();
    pcm_poll_init();

    fd = open(argv[1], O_WRONLY | O_CREAT | O_EXCL, 0666);
    if(fd < 0)
    {
        perror("open capture file error");
        free(pfds);
        snd_pcm_hw_params_free(cfg);
        snd_pcm_close(pcm);
        exit(-1);
    }

    buf = malloc(buf_bytes);
    if(NULL == buf)
    {
        perror("malloc buf error");
        free(pfds);
        snd_pcm_hw_params_free(cfg);
        snd_pcm_close(pcm);
        close(fd);
        exit(-1);
    }

    ret = snd_pcm_start(pcm);
    if(ret < 0)
    {
        fprintf(stderr, "snd_pcm_start error: %s!\n", snd_strerror(ret));
        free(buf);
        free(pfds);
        snd_pcm_hw_params_free(cfg);
        snd_pcm_close(pcm);
        close(fd);
        exit(-1);
    }

    while(1)
    {
        ret = poll(pfds, count , -1);
        if(ret < 0)
        {
            perror("poll error");
            free(buf);
            free(pfds);
            snd_pcm_hw_params_free(cfg);
            snd_pcm_close(pcm);
            close(fd);
            exit(-1);
        }

        ret = snd_pcm_poll_descriptors_revents(pcm, pfds, count, &revents);
        if(ret < 0)
        {
            fprintf(stderr, "snd_pcm_poll_descriptors_revents error: %s!\n", snd_strerror(ret));
            free(buf);
            free(pfds);
            snd_pcm_hw_params_free(cfg);
            snd_pcm_close(pcm);
            close(fd);
            exit(-1);
        }
        if(revents & POLLERR)
        {
            perror("POLLERR");
            free(buf);
            free(pfds);
            snd_pcm_hw_params_free(cfg);
            snd_pcm_close(pcm);
            close(fd);
            exit(-1);
        }
        if(revents & POLLIN)
        {
            for(read_frame = snd_pcm_avail_update(pcm); read_frame >= 1024; read_frame = snd_pcm_avail_update(pcm))
            {
                memset(buf, 0, buf_bytes);
                ret_frame = snd_pcm_readi(pcm, buf, 1024);
                if(ret_frame < 0)
                {
                    fprintf(stderr, "read pcm data error: %s!\n", snd_strerror(ret_frame));
                    free(buf);
                    free(pfds);
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
                    free(pfds);
                    snd_pcm_hw_params_free(cfg);
                    snd_pcm_close(pcm);
                    close(fd);
                    exit(-1);
                }
            }
        }
    }

    free(buf);
    free(pfds);
    snd_pcm_hw_params_free(cfg);
    snd_pcm_close(pcm);
    close(fd);
    return 0;
}