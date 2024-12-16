obj-m := DrTimer.o

KERNELDIR = /lib/modules/5.15.45Darky@Sanky-v7l/build

default:
	make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- -C $(KERNELDIR) M=$(shell pwd) modules

clean:
	make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- -C $(KERNELDIR) M=$(shell pwd) clean
