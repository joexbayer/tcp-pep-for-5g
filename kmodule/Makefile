obj-m += pepmodule.o
pepmodule-objs := pepcore.o ../lib/src/tlv.o
PWD := $(CURDIR)
MODULE = pepmodule

ccflags-y += -DCONFIG_KMODULE -I../lib/ -I./include/

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	rm -f ../lib/src/.tlv.o.cmd

logs:
	sudo dmesg | grep "PEP"

install:
	sudo sysctl -w net.ipv4.tcp_fastopen=3
	sudo insmod $(MODULE).ko
	sudo dmesg -wH

uninstall:
	sudo rmmod $(MODULE)