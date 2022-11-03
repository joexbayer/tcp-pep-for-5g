obj-m += sysmodule.o
sysmodule-objs := syscore.o
PWD := $(CURDIR)
MODULE = sysmodule

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

install:
	sudo insmod $(MODULE).ko
	sudo dmesg -wH

uninstall:
	sudo rmmod $(MODULE)
