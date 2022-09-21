obj-m += core.o
PWD := $(CURDIR)
MODULE = core

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

logs:
	sudo dmesg | grep "PEP"

install:
	sudo insmod $(MODULE).ko

uninstall:
	sudo rmmod $(MODULE)
