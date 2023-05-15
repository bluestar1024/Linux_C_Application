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

#define PCM_PLAYBACK_DEV    "hw:0,0"

typedef struct pcm_info_format{
    char chunkID[4];
    u_int32_t chunksize;
    char format[4];
}__attribute__ ((packed))pif_t;
typedef struct pcm_info_format1{
    char subchunk1ID[4];
    u_int32_t subchunk1size;
    u_int16_t audioformat;
    u_int16_t numchannels;
    u_int32_t samplerate;
    u_int32_t byterate;
    u_int16_t blockalign;
    u_int16_t bitspersample;
}__attribute__ ((packed))pif1_t;
pif1_t pi1 = {0};
typedef struct pcm_info_format2{
    char subchunk2ID[4];
    u_int32_t subchunk2size;
}__attribute__ ((packed))pif2_t;

snd_pcm_t *pcm = NULL;
snd_pcm_hw_params_t *cfg = NULL;
char *buf = NULL;
int buf_bytes = 0;
int fd = -1;

void wav_file_init(char *file)
{
    pif_t pi = {0};
    pif2_t pi2 = {0};
    int ret = 0;

    fd = open(file, O_RDONLY);
    if(fd < 0)
    {
        perror("open wav error");
        exit(-1);
    }

    ret = read(fd, &pi, sizeof(pif_t));
    if(ret != sizeof(pif_t))
    {
        perror("read wav pi error");
        close(fd);
        exit(-1);
    }
    if(strncmp("RIFF", pi.chunkID, 4) || strncmp("WAVE", pi.format, 4))
    {
        fprintf(stderr, "no wav file pi!\n");
        close(fd);
        exit(-1);
    }

    ret = read(fd, &pi1, sizeof(pif1_t));
    if(ret != sizeof(pif1_t))
    {
        perror("read wav pi1 error");
        close(fd);
        exit(-1);
    }
    if(strncmp("fmt ", pi1.subchunk1ID, 4))
    {
        fprintf(stderr, "no wav file pi1!\n");
        close(fd);
        exit(-1);
    }
    puts("<<<<wav file info>>>>\n");
    printf("file name: %s\n", file);
    printf("subchunk1size: %d\n", pi1.subchunk1size);
    printf("audioformat: %hd\n", pi1.audioformat);
    printf("numchannels: %hd\n", pi1.numchannels);
    printf("samplerate: %d\n", pi1.samplerate);
    printf("byterate: %d\n", pi1.byterate);
    printf("blockalign: %hd\n", pi1.blockalign);
    printf("bitspersample: %hd\n", pi1.bitspersample);

    buf_bytes = 1024 * pi1.blockalign;

    ret = lseek(fd, sizeof(pif_t) + 8 + pi1.subchunk1size, SEEK_SET);
    if(ret < 0)
    {
        perror("lseek pi2 error");
        close(fd);
        exit(-1);
    }

    ret = read(fd, &pi2, sizeof(pif2_t));
    if(ret != sizeof(pif2_t))
    {
        perror("read wav pi2 error");
        close(fd);
        exit(-1);
    }
    if(strncmp("data", pi2.subchunk2ID, 4))
    {
        fprintf(stderr, "no wav file pi2!\n");
        close(fd);
        exit(-1);
    }
}

void snd_playback_async_callback(snd_async_handler_t *handler)
{
    snd_pcm_t *pcm_handler = snd_async_handler_get_pcm(handler);
    unsigned int write_frame = 0;
    int ret_bytes = 0;
    int ret_frame = 0;
    int ret = 0;

    for(write_frame = snd_pcm_avail_update(pcm_handler); write_frame >= 1024; write_frame = snd_pcm_avail_update(pcm_handler))
    {
        memset(buf, 0x00, buf_bytes);
        ret_bytes = read(fd, buf, buf_bytes);
        if(ret_bytes <= 0)
        {
            if(ret_bytes < 0)
                perror("read wav data error");
            free(buf);
            snd_pcm_hw_params_free(cfg);
            snd_pcm_close(pcm_handler);
            close(fd);
            exit(-1);
        }

        ret_frame = snd_pcm_writei(pcm_handler, buf, ret_bytes / pi1.blockalign);
        if(ret_frame < 0)
        {
            fprintf(stderr, "write pcm data error: %s!\n", snd_strerror(ret_frame));
            free(buf);
            snd_pcm_hw_params_free(cfg);
            snd_pcm_close(pcm_handler);
            close(fd);
            exit(-1);
        }
        else if(ret_frame < ret_bytes / pi1.blockalign)
        {
            ret = lseek(fd, ret_frame * pi1.blockalign - ret_bytes, SEEK_CUR);
            if(ret < 0)
            {
                perror("lseek data error");
                free(buf);
                snd_pcm_hw_params_free(cfg);
                snd_pcm_close(pcm_handler);
                close(fd);
                exit(-1);
            }
        }
    }
}

void pcm_init(void)
{
    snd_async_handler_t *async_handler = NULL;
    int ret = 0;

    ret = snd_pcm_open(&pcm, PCM_PLAYBACK_DEV, SND_PCM_STREAM_PLAYBACK, 0);
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

    ret = snd_pcm_hw_params_set_channels(pcm, cfg, pi1.numchannels);
    if(ret < 0)
    {
        fprintf(stderr, "snd_pcm_hw_params_set_channels error: %s!\n", snd_strerror(ret));
        snd_pcm_hw_params_free(cfg);
        snd_pcm_close(pcm);
        exit(-1);
    }

    ret = snd_pcm_hw_params_set_rate(pcm, cfg, pi1.samplerate, 0);
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

    ret = snd_async_add_pcm_handler(&async_handler, pcm, snd_playback_async_callback, NULL);
    if(ret < 0)
    {
        fprintf(stderr, "snd_async_add_pcm_handler error: %s!\n", snd_strerror(ret));
        snd_pcm_hw_params_free(cfg);
        snd_pcm_close(pcm);
        exit(-1);
    }
}

int main(int argc, char **argv)
{
    unsigned int write_frame = 0;
    int ret_bytes = 0;
    int ret_frame = 0;
    int ret = 0;

    if(2 != argc)
    {
        fprintf(stderr, "usage: %s <wav file>!\n", argv[0]);
        exit(-1);
    }

    wav_file_init(argv[1]);
    pcm_init();

    buf = malloc(buf_bytes);
    if(NULL == buf)
    {
        perror("malloc error");
        snd_pcm_hw_params_free(cfg);
        snd_pcm_close(pcm);
        close(fd);
        exit(-1);
    }

    for(write_frame = snd_pcm_avail_update(pcm); write_frame >= 1024; write_frame = snd_pcm_avail_update(pcm))
    {
        memset(buf, 0x00, buf_bytes);
        ret_bytes = read(fd, buf, buf_bytes);
        if(ret_bytes <= 0)
        {
            if(ret_bytes < 0)
                perror("read wav data error");
            free(buf);
            snd_pcm_hw_params_free(cfg);
            snd_pcm_close(pcm);
            close(fd);
            exit(-1);
        }

        ret_frame = snd_pcm_writei(pcm, buf, ret_bytes / pi1.blockalign);
        if(ret_frame < 0)
        {
            fprintf(stderr, "write pcm data error: %s!\n", snd_strerror(ret_frame));
            free(buf);
            snd_pcm_hw_params_free(cfg);
            snd_pcm_close(pcm);
            close(fd);
            exit(-1);
        }
        else if(ret_frame < ret_bytes / pi1.blockalign)
        {
            ret = lseek(fd, ret_frame * pi1.blockalign - ret_bytes, SEEK_CUR);
            if(ret < 0)
            {
                perror("lseek data error");
                free(buf);
                snd_pcm_hw_params_free(cfg);
                snd_pcm_close(pcm);
                close(fd);
                exit(-1);
            }
        }
    }

    while(1)
        sleep(1);

    free(buf);
    snd_pcm_hw_params_free(cfg);
    snd_pcm_close(pcm);
    close(fd);
    return 0;
}