cmake_minimum_required(VERSION 3.5)
project(MQTTCLIENT)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

#主程序
add_executable(mqttclient mqttclient.c)
target_include_directories(mqttclient PRIVATE /home/dong/CApplication/tools/paho.mqtt/include)
target_link_directories(mqttclient PRIVATE /home/dong/CApplication/tools/paho.mqtt/lib)
target_link_libraries(mqttclient PRIVATE paho-mqtt3c)

#音频播放程序
add_executable(pcm_async_playback_ctl_mixer_mqtt pcm_async_playback_ctl_mixer_mqtt.c)
target_link_libraries(pcm_async_playback_ctl_mixer_mqtt PRIVATE asound)

#视频监控程序
add_executable(v4l2_mqtt v4l2_mqtt.c)

#文字显示资源列表程序
add_executable(lcd_freetype_show_char_mqtt lcd_freetype_show_char_mqtt.c)
target_include_directories(lcd_freetype_show_char_mqtt PRIVATE /home/dong/CApplication/tools/freetype/include/freetype2)
target_link_directories(lcd_freetype_show_char_mqtt PRIVATE /home/dong/CApplication/tools/freetype/lib /home/dong/CApplication/tools/zlib/lib /home/dong/CApplication/tools/libpng_1637/lib)
target_link_libraries(lcd_freetype_show_char_mqtt PRIVATE freetype z png m)

#触摸跟随手指程序
add_executable(lcd_followfingers_mqtt lcd_followfingers_mqtt.c)
