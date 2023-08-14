Minimalistic C++ wrapper around V4L2
====================================

Build the demo
--------------

You need the V4L2 development package. On Debian/Ubuntu:
```
$ sudo apt-get install libv4l2-dev
```

Then:
```
$ g++ -std=c++11 grab.cpp webcam.cpp -o grab -lv4l2 -ljpeg
```
For cross compile:
```
/home/caphuong/linux_trainning/toolchain/gcc-linaro-6.5.0-2018.12-x86_64_arm-linux-gnueabihf/bin/arm-linux-gnueabihf-g++ -std=c++11 grab.cpp webcam.cpp -o grab -static -lv4l2 -lv4lconvert -ljpeg -L/home/caphuong/my_rootfs/usr/local/lib/ -I/home/caphuong/my_rootfs/usr/local/include/

```

Calling `./grab` acquires one image from `/dev/video0` and saves it as
`frame.jpg`.

Convert to png
```
ffmpeg -i frame.ppm out.png
```


Query camera resolution

```
v4l2-ctl -d /dev/video0 --list-formats-ext
```