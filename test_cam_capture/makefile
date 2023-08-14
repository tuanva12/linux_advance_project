

all:
	/home/caphuong/linux_trainning/toolchain/gcc-linaro-6.5.0-2018.12-x86_64_arm-linux-gnueabihf/bin/arm-linux-gnueabihf-g++ -std=c++11 grab.cpp webcam.cpp -o grab -static -lv4l2 -ljpeg -L/home/caphuong/my_rootfs/usr/local/lib/ -I/home/caphuong/my_rootfs/usr/local/include/
	scp grab debian@192.168.15.2:/home/debian/anhtuan/
