# Sử dụng lib mqtt paho.
Sử dụng thư viện tại: https://github.com/eclipse/paho.mqtt.cpp

Build thư viện theo hướng dẫn tại readme trong repo.

Sử dụng dinamic library: https://cylab.be/blog/234/creating-a-dynamic-library-in-c

## Command test mosquito

mosquitto_pub -h 192.168.15.5 -t 0:c:29:ff:dc:4a/command -m "{\"type\":\"version\"}"

mosquitto_sub -h 192.168.15.5 -t 0:c:29:ff:dc:4a/command

# install json library

1. git clone https://github.com/nlohmann/json.git
2. mkdir build
3. cd build
4. cmake ..
5. make
6. make install

Cần cross complier g++ khi build cho BBB

# FFmpeg

CMD capture image: ffmpeg -f video4linux2 -i /dev/video0 -vframes 1  -video_size 640x480 test.jpeg