obj-m := led-driver.o

MODULES = $(shell pwd)/linux

CR_C = $(shell pwd)/toolchain/bin/arm-buildroot-linux-gnueabihf-

SSH_TARGET = "pi@192.168.1.23"
OUT_MODULE = "led-driver.ko"

all:
	make ARCH=arm CROSS_COMPILE=$(CR_C) -C $(MODULES) M=$$PWD modules

clean:
	make -C $(MODULES) M=$$PWD clean

scp:
	scp $(OUT_MODULE) $(SSH_TARGET):~

insmod:
	ssh $(SSH_TARGET) 'sudo insmod ~/$(OUT_MODULE)'

rmmod:
	ssh $(SSH_TARGET) 'sudo rmmod $(OUT_MODULE)'
