ifneq ($(KERNELRELEASE),)
obj-m := proc_rw.o

else
KDIR := /lib/modules/$(shell uname -r)/build
PWE := $(shell pwd)

default:
	$(MAKE) -C $(KDIR) M=$(PWD) modules
endif

clean:
	rm -rf *.o
	rm -rf *.ko
	rm -rf *~
	rm -rf modules.order
	rm -rf Module.symvers

