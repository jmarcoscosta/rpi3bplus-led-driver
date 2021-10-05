obj-m := led-driver.o

MODULES = $(shell pwd)/linux

CR_C = $(shell pwd)/toolchain/bin/arm-buildroot-linux-gnueabihf-

all:
	make ARCH=arm CROSS_COMPILE=$(CR_C) -C $(MODULES) M=$$PWD modules

clean:
	make -C $(MODULES) M=$$PWD clean
