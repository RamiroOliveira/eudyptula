obj-m += hello_world.o

ifndef KDIR
KDIR = /lib/modules/$(shell uname -r)/build
endif

CFLAGS_hello_world.o := -DDEBUG

all:
	make -C $(KDIR) M=$$PWD modules V=1 
clean:
	make -C $(KDIR) M=$$PWD clean V=1
