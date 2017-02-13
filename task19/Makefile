obj-m += hello_world.o

ifndef KDIR
KDIR = /lib/modules/$(shell uname -r)/build
endif

ccflags-y := -O1 -g -ggdb -Wall -DDEBUG

CC := gcc

all:
	@$(MAKE) -C $(KDIR) M=$$PWD modules V=0 
clean:
	@$(MAKE) -C $(KDIR) M=$$PWD clean V=0
