obj-m += hello_world.o

ifndef KDIR
KDIR = /lib/modules/$(shell uname -r)/build
endif

ccflags-y := -O1 -g -ggdb -Wall -DDEBUG

all:
	make -C $(KDIR) M=$$PWD modules V=0 
clean:
	make -C $(KDIR) M=$$PWD clean V=0
