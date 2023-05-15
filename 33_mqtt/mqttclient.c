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
#include <signal.h>
#include <termios.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include "MQTTClient.h"

#define SERVER_ADDR                             "tcp://iot.ranye-iot.net:1883"

#define CLIENT_ID                               "bulestar_1_id"
#define USER_NAME                               "bulestar_1"
#define PASS_WORD                               "123"

#define WILL_TOPIC                              "bulestar/will_topic"
#define LED_TOPIC                               "bulestar/led_topic"
#define TEMP_TOPIC                              "bulestar/temp_topic"
#define AUDIO_PLAY_TOPIC                        "bulestar/audio_play_topic"
#define AUDIO_CAPTURE_TOPIC                     "bulestar/audio_capture_topic"
#define VIDEO_PLAY_TOPIC                        "bulestar/video_play_topic"
#define VIDEO_CAPTURE_TOPIC                     "bulestar/video_capture_topic"
#define PICTURE_PLAY_TOPIC                      "bulestar/picture_play_topic"
#define PICTURE_CAPTURE_TOPIC                   "bulestar/picture_capture_topic"
#define CHAR_TOPIC                              "bulestar/char_topic"
#define TOUCH_TOPIC                             "bulestar/touch_topic"

#define AUDIO_FORMAT1_INIT_NUM                  1
#define AUDIO_FORMAT2_INIT_NUM                  1
#define AUDIO_FORMAT3_INIT_NUM                  0
#define VIDEO_INIT_NUM                          0
#define PICTURE_BMP_INIT_NUM                    1
#define PICTURE_JPEG_INIT_NUM                   3
#define PICTURE_PNG_INIT_NUM                    1
#define PICTURE_VERTICAL_BMP_INIT_NUM           2

int child_num = 0;
char *audio_format1_list[10] = {NULL};
char *audio_format2_list[10] = {NULL};
char audio_format3_list[10][100] = {0};
char video_list[10][100] = {0};
char picture_bmp_list[10][100] = {0};
char picture_jpeg_list[10][100] = {0};
char picture_png_list[10][100] = {0};
char picture_vertical_bmp_list[10][100] = {0};

void list_init(void)
{
    audio_format1_list[0] = "开不了口-周杰伦.wav";
    audio_format2_list[0] = "从前说-小阿七.wav";
    strcpy(picture_bmp_list[0], "火车.bmp");
    strcpy(picture_jpeg_list[0], "看书-章若楠.jpeg");
    strcpy(picture_jpeg_list[1], "微笑-章若楠.jpeg");
    strcpy(picture_jpeg_list[2], "紫草.jpeg");
    strcpy(picture_png_list[0], "玄幻星球.png");
    strcpy(picture_vertical_bmp_list[0], "蘑菇树.bmp");
    strcpy(picture_vertical_bmp_list[1], "花园旧迹.bmp");
}

int add_child(char *cmd)
{
    pid_t pid = 0;

    if(NULL == cmd)
        fprintf(stderr, "cmd error!\n");

    pid = fork();
    if(pid < 0)
        fprintf(stderr, "fork error!\n");
    else if(0 == pid)
    {
        execl("/bin/sh", "sh", "-c", cmd, NULL);
        fprintf(stderr, "execl error!\n");
        exit(-1);
    }

    child_num++;
    return pid;
}

void connectlose(void *context, char *cause)
{
    fprintf(stderr, "connect lose!\n");
    printf("cause: %s\n", cause);
}

int messagearrive(void *context, char *topicName, int topicLen, MQTTClient_message *message)
{
    unsigned short *screen_base = NULL;
    unsigned int screen_size = 0;
    int lcd_fd = -1;
    struct fb_var_screeninfo fb_var = {0};
    struct fb_fix_screeninfo fb_fix = {0};

    int audio_play_fd = -1;
    int audio_play_i = 0;
    char audio_play_cmd[100] = {0};
    static int audio_play_format3_count = AUDIO_FORMAT3_INIT_NUM;

    static int audio_capture_format3_count = 0;
    char audio_capture_cmd[100] = {0};
    char *audio_capture_pointer = NULL;
    static int audio_capture_pid = 0;
    static int audio_capture_status = 0;

    static int video_play_count = VIDEO_INIT_NUM;
    int video_play_i = 0;
    char video_play_cmd[100] = {0};

    char *video_capture_pointer1 = NULL;
    char *video_capture_pointer2 = NULL;
    char video_capture_cmd[100] = {0};
    static int video_capture_pid = 0;
    static int video_capture_count = 0;
    int video_capture_i = 0;
    char video_capture_file_name[100] = {0};

    static int picture_play_bmp_count = PICTURE_BMP_INIT_NUM;
    static int picture_play_jpeg_count = PICTURE_JPEG_INIT_NUM;
    static int picture_play_png_count = PICTURE_PNG_INIT_NUM;
    static int picture_play_vertical_bmp_count = PICTURE_VERTICAL_BMP_INIT_NUM;
    int picture_play_i = 0;
    char picture_play_cmd[100] = {0};

    static int picture_capture_jpeg_count = 0;
    char picture_capture_cmd[100] = {0};

    int char_fd = -1;

    static int touch_pid = 0;
    char touch_cmd[100] = {0};

    if(!strcmp(topicName, LED_TOPIC))
    {
        if(!strcmp(message->payload, "心跳"))
            system("echo heartbeat > /sys/class/leds/sys-led/trigger");
        else if(!strcmp(message->payload, "亮"))
        {
            system("echo none > /sys/class/leds/sys-led/trigger");
            system("echo 1 > /sys/class/leds/sys-led/brightness");
        }
        else if(!strcmp(message->payload, "灭"))
        {
            system("echo none > /sys/class/leds/sys-led/trigger");
            system("echo 0 > /sys/class/leds/sys-led/brightness");
        }
    }
    else if(!strcmp(topicName, AUDIO_PLAY_TOPIC))
    {
        if((!strcmp(message->payload, "暂停")) || (!strcmp(message->payload, "继续")))
        {
            audio_play_fd = open("input.txt", O_WRONLY);
            write(audio_play_fd, " ", 1);
            close(audio_play_fd);
        }
        else if(!strcmp(message->payload, "结束"))
        {
            audio_play_fd = open("input.txt", O_WRONLY);
            write(audio_play_fd, "q", 1);
            close(audio_play_fd);
        }
        else if(!strcmp(message->payload, "+"))
        {
            audio_play_fd = open("input.txt", O_WRONLY);
            write(audio_play_fd, "wwwwwwwwww", 10);
            close(audio_play_fd);
        }
        else if(!strcmp(message->payload, "-"))
        {
            audio_play_fd = open("input.txt", O_WRONLY);
            write(audio_play_fd, "ssssssssss", 10);
            close(audio_play_fd);
        }
        else
        {
            for(; audio_play_i < AUDIO_FORMAT1_INIT_NUM; audio_play_i++)
            {
                if(!strncmp(audio_format1_list[audio_play_i], message->payload, strlen(message->payload)))
                {
                    memset(audio_play_cmd, 0, sizeof(audio_play_cmd));
                    sprintf(audio_play_cmd, "./pcm_async_playback_ctl_mixer_mqtt %s", audio_format1_list[audio_play_i]);
                    add_child(audio_play_cmd);
                    goto audio_play_end;
                }
            }
            for(audio_play_i = 0; audio_play_i < AUDIO_FORMAT2_INIT_NUM; audio_play_i++)
            {
                if(!strncmp(audio_format2_list[audio_play_i], message->payload, strlen(message->payload)))
                {
                    memset(audio_play_cmd, 0, sizeof(audio_play_cmd));
                    sprintf(audio_play_cmd, "aplay %s", audio_format2_list[audio_play_i]);
                    add_child(audio_play_cmd);
                    goto audio_play_end;
                }
            }
            for(audio_play_i = 0; audio_play_i < audio_play_format3_count; audio_play_i++)
            {
                if(!strncmp(audio_format3_list[audio_play_i], message->payload, strlen(message->payload)))
                {
                    memset(audio_play_cmd, 0, sizeof(audio_play_cmd));
                    sprintf(audio_play_cmd, "aplay -f cd %s", audio_format3_list[audio_play_i]);
                    add_child(audio_play_cmd);
                    goto audio_play_end;
                }
            }
            fprintf(stderr, "no such file!\n");
        }
    }
    else if(!strcmp(topicName, AUDIO_CAPTURE_TOPIC))
    {
        if((!strcmp(message->payload, "录音")) && (!audio_capture_status))
        {
            audio_capture_status = 1;
            audio_capture_format3_count++;
            memset(audio_capture_cmd, 0, sizeof(audio_capture_cmd));
            sprintf(audio_capture_cmd, "./pcm_poll_capture audio_capture%d.wav", audio_capture_format3_count);
            audio_capture_pid = add_child(audio_capture_cmd);
            printf("audio capture start to audio_capture%d.wav!\n", audio_capture_format3_count);
            sprintf(audio_format3_list[audio_play_format3_count], "audio_capture%d.wav", audio_capture_format3_count);
            audio_play_format3_count++;
        }
        else if((!strcmp(message->payload, "结束")) && audio_capture_status)
        {
            audio_capture_status = 0;
            memset(audio_capture_cmd, 0, sizeof(audio_capture_cmd));
            sprintf(audio_capture_cmd, "kill -9 %d", audio_capture_pid);
            system(audio_capture_cmd);
            puts("audio capture end, file generated!");
        }
        else if(!audio_capture_status)
        {
            audio_capture_status = 1;
            memset(audio_capture_cmd, 0, sizeof(audio_capture_cmd));
            sprintf(audio_capture_cmd, "./pcm_poll_capture %s.wav", message->payload);
            audio_capture_pid = add_child(audio_capture_cmd);
            printf("audio capture start to %s.wav!\n", message->payload);
            sprintf(audio_format3_list[audio_play_format3_count], "%s.wav", message->payload);
            audio_play_format3_count++;
        }
    }
    else if(!strcmp(topicName, VIDEO_PLAY_TOPIC))
    {
        for(; video_play_i < video_play_count; video_play_i++)
        {
            if(!strncmp(video_list[video_play_i], message->payload, strlen(message->payload)))
            {
                memset(video_play_cmd, 0, sizeof(video_play_cmd));
                sprintf(video_play_cmd, "gst-play-1.0 %s", video_list[video_play_i]);
                add_child(video_play_cmd);
                goto video_play_end;
            }
        }
        fprintf(stderr, "no such file!\n");
    }
    else if(!strcmp(topicName, VIDEO_CAPTURE_TOPIC))
    {
        if(!strcmp(message->payload, "监控"))
            video_capture_pid = add_child("./v4l2_mqtt");
        else if(!strcmp(message->payload, "结束"))
        {
            memset(video_capture_cmd, 0, sizeof(video_capture_cmd));
            sprintf(video_capture_cmd, "kill -10 %d", video_capture_pid);
            system(video_capture_cmd);
            puts("video capture real time end!");
        }
        else if(!strncmp(message->payload, "录像", 4))
        {
            video_capture_pointer1 = message->payload + 6;
            for(video_capture_pointer2 = video_capture_pointer1; *video_capture_pointer2 != '+'; video_capture_pointer2++);
            system("camera_settings /dev/video1 RGB565 320 240 30");
            if(video_capture_pointer2 == video_capture_pointer1)
            {
                video_capture_count++;
                memset(video_capture_cmd, 0, sizeof(video_capture_cmd));
                sprintf(video_capture_cmd, "ffmpeg -t %s -pix_fmt rgb565le -i /dev/video1 video_capture%d.avi", video_capture_pointer2 + 1, video_capture_count);
                add_child(video_capture_cmd);
                printf("video capture start to video_capture%d.avi!\n", video_capture_count);
                sprintf(video_list[video_play_count], "video_capture%d.avi", video_capture_count);
                video_play_count++;
            }
            else
            {
                memset(video_capture_file_name, 0, sizeof(video_capture_file_name));
                memset(video_capture_cmd, 0, sizeof(video_capture_cmd));
                snprintf(video_capture_file_name, video_capture_pointer2 - video_capture_pointer1 + 1, "%s", video_capture_pointer1);
                sprintf(video_capture_cmd, "ffmpeg -t %s -pix_fmt rgb565le -i /dev/video1 %s.avi", video_capture_pointer2 + 1, video_capture_file_name);
                add_child(video_capture_cmd);
                printf("video capture start to %s.avi!\n", video_capture_file_name);;
                sprintf(video_list[video_play_count], "%s.avi", video_capture_file_name);
                video_play_count++;
            }
        }
    }
    else if(!strcmp(topicName, PICTURE_PLAY_TOPIC))
    {
        for(; picture_play_i < picture_play_bmp_count; picture_play_i++)
        {
            if(!strncmp(picture_bmp_list[picture_play_i], message->payload, strlen(message->payload)))
            {
                memset(picture_play_cmd, 0, sizeof(picture_play_cmd));
                sprintf(picture_play_cmd, "./lcd_bmp_show %s", picture_bmp_list[picture_play_i]);
                system(picture_play_cmd);
                goto picture_play_end;
            }
        }
        for(picture_play_i = 0; picture_play_i < picture_play_jpeg_count; picture_play_i++)
        {
            if(!strncmp(picture_jpeg_list[picture_play_i], message->payload, strlen(message->payload)))
            {
                memset(picture_play_cmd, 0, sizeof(picture_play_cmd));
                sprintf(picture_play_cmd, "./lcd_jpeg_show %s", picture_jpeg_list[picture_play_i]);
                system(picture_play_cmd);
                goto picture_play_end;
            }
        }
        for(picture_play_i = 0; picture_play_i < picture_play_png_count; picture_play_i++)
        {
            if(!strncmp(picture_png_list[picture_play_i], message->payload, strlen(message->payload)))
            {
                memset(picture_play_cmd, 0, sizeof(picture_play_cmd));
                sprintf(picture_play_cmd, "./lcd_png_show %s", picture_png_list[picture_play_i]);
                system(picture_play_cmd);
                goto picture_play_end;
            }
        }
        for(picture_play_i = 0; picture_play_i < picture_play_vertical_bmp_count; picture_play_i++)
        {
            if(!strncmp(picture_vertical_bmp_list[picture_play_i], message->payload, strlen(message->payload)))
            {
                memset(picture_play_cmd, 0, sizeof(picture_play_cmd));
                sprintf(picture_play_cmd, "./lcd_vertical_display_bmp %s", picture_vertical_bmp_list[picture_play_i]);
                system(picture_play_cmd);
                goto picture_play_end;
            }
        }
        fprintf(stderr, "no such file!\n");
    }
    else if(!strcmp(topicName, PICTURE_CAPTURE_TOPIC))
    {
        if(!strcmp(message->payload, "拍照"))
        {
            picture_capture_jpeg_count++;
            memset(picture_capture_cmd, 0, sizeof(picture_capture_cmd));
            sprintf(picture_capture_cmd, "gst-launch-1.0 imxv4l2src num-buffers=1 device=/dev/video1 ! jpegenc ! filesink location=picture_capture%d.jpg", picture_capture_jpeg_count);
            system(picture_capture_cmd);
            printf("picture capture start to picture_capture%d.jpg!\n", picture_capture_jpeg_count);
            sprintf(picture_jpeg_list[picture_play_jpeg_count], "picture_capture%d.jpg", picture_capture_jpeg_count);
            picture_play_jpeg_count++;
        }
        else
        {
            memset(picture_capture_cmd, 0, sizeof(picture_capture_cmd));
            sprintf(picture_capture_cmd, "gst-launch-1.0 imxv4l2src num-buffers=1 device=/dev/video1 ! jpegenc ! filesink location=%s.jpg", message->payload);
            system(picture_capture_cmd);
            printf("picture capture start to %s.jpg!\n", message->payload);
            sprintf(picture_jpeg_list[picture_play_jpeg_count], "%s.jpg", message->payload);
            picture_play_jpeg_count++;
        }
    }
    else if(!strcmp(topicName, CHAR_TOPIC))
    {
        if(!strcmp(message->payload, "资源"))
        {
            char_fd = open("list.txt", O_WRONLY | O_CREAT);
            system("ls *.wav > list.txt");
            system("ls *.avi >> list.txt");
            system("ls *.bmp >> list.txt");
            system("ls *.jpg >> list.txt");
            system("ls *.jpeg >> list.txt");
            system("ls *.png >> list.txt");
            close(char_fd);
            system("./lcd_freetype_show_char_mqtt /usr/share/fonts/ttf/SourceHanSansCN-Regular.otf 0");
        }
    }
    else if(!strcmp(topicName, TOUCH_TOPIC))
    {
        if(!strcmp(message->payload, "触摸"))
            touch_pid = add_child("./lcd_followfingers_mqtt /dev/input/touchscreen0");
        else if(!strcmp(message->payload, "结束"))
        {
            memset(touch_cmd, 0, sizeof(touch_cmd));
            sprintf(touch_cmd, "kill -9 %d", touch_pid);
            system(touch_cmd);
            puts("touch end!");
        }
    }

picture_play_end:
video_play_end:
audio_play_end:
    MQTTClient_freeMessage(&message);
    MQTTClient_free(topicName);
    return 1;
}

int main(void)
{
    MQTTClient client = {0};
    MQTTClient_message message = MQTTClient_message_initializer;
    MQTTClient_willOptions willoptions = MQTTClient_willOptions_initializer;
    MQTTClient_connectOptions connectoptions = MQTTClient_connectOptions_initializer;
    char temp_buf[20] = {0};
    int fd = -1;
    int ret = 0;
    int i = 0;
    int child_wait_num = 0;

    list_init();

    if(access("input.txt", F_OK))
    {
        if(mkfifo("input.txt", 0666))
        {
            perror("mkfifo error!\n");
            exit(-1);
        }
    }
    
    ret = MQTTClient_create(&client, SERVER_ADDR, CLIENT_ID, MQTTCLIENT_PERSISTENCE_NONE, NULL);
    if(ret != MQTTCLIENT_SUCCESS)
    {
        fprintf(stderr, "MQTTClient_create error!\n");
        goto MQTTClient_create_err;
    }

    ret = MQTTClient_setCallbacks(client, NULL, connectlose, messagearrive, NULL);
    if(ret != MQTTCLIENT_SUCCESS)
    {
        fprintf(stderr, "MQTTClient_setCallbacks error!\n");
        goto MQTTClient_setCallbacks_err;
    }

    willoptions.topicName = WILL_TOPIC;
    willoptions.message = "unexpected disconnect";
    willoptions.retained = 1;
    willoptions.qos = 0;

    connectoptions.keepAliveInterval = 30;
    connectoptions.cleansession = 0;
    connectoptions.will = &willoptions;
    connectoptions.username = USER_NAME;
    connectoptions.password = PASS_WORD;
    ret = MQTTClient_connect(client, &connectoptions);
    if(ret != MQTTCLIENT_SUCCESS)
    {
        fprintf(stderr, "MQTTClient_connect error!\n");
        goto MQTTClient_connect_err;
    }
    puts("MQTT connect success!");

    message.payload = "online";
    message.payloadlen = strlen(message.payload);
    message.qos = 0;
    message.retained = 1;
    ret = MQTTClient_publishMessage(client, WILL_TOPIC, &message, NULL);
    if(ret != MQTTCLIENT_SUCCESS)
    {
        fprintf(stderr, "MQTTClient_publishMessage error!\n");
        goto MQTTClient_publishMessage_will_err;
    }

    ret = MQTTClient_subscribe(client, LED_TOPIC, 0);
    if(ret != MQTTCLIENT_SUCCESS)
    {
        fprintf(stderr, "MQTTClient_subscribe led error!\n");
        goto MQTTClient_subscribe_led_err;
    }

    ret = MQTTClient_subscribe(client, AUDIO_PLAY_TOPIC, 0);
    if(ret != MQTTCLIENT_SUCCESS)
    {
        fprintf(stderr, "MQTTClient_subscribe play audio error!\n");
        goto MQTTClient_subscribe_play_audio_err;
    }

    ret = MQTTClient_subscribe(client, AUDIO_CAPTURE_TOPIC, 0);
    if(ret != MQTTCLIENT_SUCCESS)
    {
        fprintf(stderr, "MQTTClient_subscribe capture audio error!\n");
        goto MQTTClient_subscribe_capture_audio_err;
    }

    ret = MQTTClient_subscribe(client, VIDEO_PLAY_TOPIC, 0);
    if(ret != MQTTCLIENT_SUCCESS)
    {
        fprintf(stderr, "MQTTClient_subscribe play video error!\n");
        goto MQTTClient_subscribe_play_video_err;
    }

    ret = MQTTClient_subscribe(client, VIDEO_CAPTURE_TOPIC, 0);
    if(ret != MQTTCLIENT_SUCCESS)
    {
        fprintf(stderr, "MQTTClient_subscribe capture video error!\n");
        goto MQTTClient_subscribe_capture_video_err;
    }

    ret = MQTTClient_subscribe(client, PICTURE_PLAY_TOPIC, 0);
    if(ret != MQTTCLIENT_SUCCESS)
    {
        fprintf(stderr, "MQTTClient_subscribe play picture error!\n");
        goto MQTTClient_subscribe_play_picture_err;
    }

    ret = MQTTClient_subscribe(client, PICTURE_CAPTURE_TOPIC, 0);
    if(ret != MQTTCLIENT_SUCCESS)
    {
        fprintf(stderr, "MQTTClient_subscribe capture picture error!\n");
        goto MQTTClient_subscribe_capture_picture_err;
    }

    ret = MQTTClient_subscribe(client, CHAR_TOPIC, 0);
    if(ret != MQTTCLIENT_SUCCESS)
    {
        fprintf(stderr, "MQTTClient_subscribe char error!\n");
        goto MQTTClient_subscribe_char_err;
    }

    ret = MQTTClient_subscribe(client, TOUCH_TOPIC, 0);
    if(ret != MQTTCLIENT_SUCCESS)
    {
        fprintf(stderr, "MQTTClient_subscribe touch error!\n");
        goto MQTTClient_subscribe_touch_err;
    }

    while(1)
    {
        fd = open("/sys/class/thermal/thermal_zone0/temp", O_RDONLY);
        read(fd, temp_buf, sizeof(temp_buf));
        close(fd);

        message.payload = temp_buf;
        message.payloadlen = strlen(temp_buf);
        message.qos = 0;
        message.retained = 1;
        ret = MQTTClient_publishMessage(client, TEMP_TOPIC, &message, NULL);
        if(ret != MQTTCLIENT_SUCCESS)
        {
            fprintf(stderr, "MQTTClient_publishMessage temp error!\n");
            goto MQTTClient_publishMessage_temp_err;
        }

        for(i = 0; i < child_num; i++)
        {
            ret = waitpid(-1, NULL, WNOHANG);
            if(ret < 0)
            {
                fprintf(stderr, "waitpid error!\n");
                goto child_wait_err;
            }
            else if(ret > 0)
                child_wait_num++;
        }
        child_num -= child_wait_num;
        child_wait_num = 0;

        sleep(30);
    }
    return 0;

child_wait_err:
MQTTClient_publishMessage_temp_err:
    MQTTClient_unsubscribe(client, TOUCH_TOPIC);
MQTTClient_subscribe_touch_err:
    MQTTClient_unsubscribe(client, CHAR_TOPIC);
MQTTClient_subscribe_char_err:
    MQTTClient_unsubscribe(client, PICTURE_CAPTURE_TOPIC);
MQTTClient_subscribe_capture_picture_err:
    MQTTClient_unsubscribe(client, PICTURE_PLAY_TOPIC);
MQTTClient_subscribe_play_picture_err:
    MQTTClient_unsubscribe(client, VIDEO_CAPTURE_TOPIC);
MQTTClient_subscribe_capture_video_err:
    MQTTClient_unsubscribe(client, VIDEO_PLAY_TOPIC);
MQTTClient_subscribe_play_video_err:
    MQTTClient_unsubscribe(client, AUDIO_CAPTURE_TOPIC);
MQTTClient_subscribe_capture_audio_err:
    MQTTClient_unsubscribe(client, AUDIO_PLAY_TOPIC);
MQTTClient_subscribe_play_audio_err:
    MQTTClient_unsubscribe(client, LED_TOPIC);
MQTTClient_subscribe_led_err:
MQTTClient_publishMessage_will_err:
    MQTTClient_disconnect(client, 1000);
MQTTClient_connect_err:
MQTTClient_setCallbacks_err:
    MQTTClient_destroy(&client);
MQTTClient_create_err:
    return -1;
}