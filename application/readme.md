# Sử dụng lib mqtt paho.
Sử dụng thư viện tại: https://github.com/eclipse/paho.mqtt.cpp

Build thư viện theo hướng dẫn tại readme trong repo.

Sử dụng dinamic library: https://cylab.be/blog/234/creating-a-dynamic-library-in-c

BUILD:
1. Build c
```
$ cd application/library/
$ git clone https://github.com/eclipse/paho.mqtt.c.git
$ cd paho.mqtt.c
$ git checkout v1.3.8
$ cmake -Bbuild -H. -DPAHO_ENABLE_TESTING=OFF -DPAHO_BUILD_STATIC=ON \
-DPAHO_WITH_SSL=ON -DPAHO_HIGH_PERFORMANCE=ON \
-DCMAKE_INSTALL_PREFIX=./build/_install \
-DCMAKE_C_COMPILER=/home/caphuong/linux_advance_project/application/library/gcc-linaro-6.5.0-2018.12-x86_64_arm-linux-gnueabihf/bin/arm-linux-gnueabihf-gcc
$ sudo cmake --build build --target install
$ sudo ldconfig
```
2. Build c++
```
$ git clone https://github.com/eclipse/paho.mqtt.cpp
$ cd paho.mqtt.cpp

$ cmake -Bbuild -H. -DPAHO_BUILD_STATIC=ON \
-DPAHO_BUILD_DOCUMENTATION=TRUE -DPAHO_BUILD_SAMPLES=TRUE \
-DCMAKE_CXX_COMPILER=/home/caphuong/linux_advance_project/application/library/gcc-linaro-6.5.0-2018.12-x86_64_arm-linux-gnueabihf/bin/arm-linux-gnueabihf-g++ \
-DPAHO_WITH_SSL=OFF \
-DCMAKE_PREFIX_PATH=/home/caphuong/linux_advance_project/application/library/paho.mqtt.c/build/_install
$ sudo cmake --build build --target install
$ sudo ldconfig
```

## Command test mosquito

mosquitto_pub -h 192.168.15.5 -t 60:e8:5b:fc:f3:b4/command -m "{\"type\":\"version\"}"

mosquitto_sub -h 192.168.15.5 -t 0:c:29:ff:dc:4a/command

# install json library

1. git clone https://github.com/nlohmann/json.git
2. mkdir build
3. cd build
4. cmake ..
5. make
6. make install


# FFmpeg

Install https://github.com/FFmpeg/FFmpeg/blob/master/INSTALL.md

CMD capture image: ffmpeg -f video4linux2 -i /dev/video0 -vframes 1  -video_size 640x480 test.jpeg