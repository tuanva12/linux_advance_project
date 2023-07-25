# Sử dụng lib mqtt paho.
Sử dụng thư viện tại: https://github.com/eclipse/paho.mqtt.cpp

Build thư viện theo hướng dẫn tại readme trong repo.

Sử dụng dinamic library: https://cylab.be/blog/234/creating-a-dynamic-library-in-c

# FFmpeg

CMD capture image: ffmpeg -f video4linux2 -i /dev/video0 -vframes 1  -video_size 640x480 test.jpeg