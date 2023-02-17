CURRENT = $(shell uname -r)
KDIR = /lib/modules/$(CURRENT)/build
PWD = $(shell pwd)
TARGET = md
obj-m := $(TARGET).o
default: 
	$(MAKE) -C $(KDIR) M=$(PWD) modules
getter:
	gcc getter.c -Wall -Werror -o getter.exe
clean:
	@rm -f *.o .*.cmd .*.flags *.mod.c *.order
	@rm -f .*.*.cmd *~ *.*~ TODO.*
	@rm -fR .tmp*
	@rm -rf .tmp_versions
disclean: clean
	@rm -f *.ko *.symvers *.mod *.exe
