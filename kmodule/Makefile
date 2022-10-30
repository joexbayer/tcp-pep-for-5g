obj-m += pepmodule.o
pepmodule-objs := pepcore.o ../lib/src/tlv.o
PWD := $(CURDIR)
MODULE = pepmodule

ccflags-y += -DCONFIG_KMODULE -I../lib/include

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

logs:
	sudo dmesg | grep "PEP"

install:
	sudo insmod $(MODULE).ko
	sudo dmesg -wH

uninstall:
	sudo rmmod $(MODULE)