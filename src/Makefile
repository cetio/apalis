KDIR = /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)
obj-m += apalis.o

all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean