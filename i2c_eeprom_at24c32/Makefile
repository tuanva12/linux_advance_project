obj-m += myat24c32.o

PWD := $(shell pwd)
CROSS=/home/caphuong/linux_trainning/bbb/kernelbuildscripts/dl/gcc-8.5.0-nolibc/arm-linux-gnueabi/bin/arm-linux-gnueabi-
KER_DIR=/home/caphuong/linux_trainning/bbb/kernelbuildscripts/KERNEL

all:
	make ARCH=arm CROSS_COMPILE=$(CROSS) -C $(KER_DIR) M=$(PWD) modules
	scp myat24c32.ko debian@192.168.15.2:~/

clean:
	make -C $(KER_DIR) M=$(PWD) clean