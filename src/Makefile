# Comment/uncomment the following line to enable/disable debugging
#DEBUG = y
ifeq ($(DEBUG),y)
  DEBFLAGS = -O -g # "-O" is needed to expand inlines
else
  DEBFLAGS = -O2
endif

EXTRA_CFLAGS += $(DEBFLAGS)
EXTRA_CFLAGS += -I$(LDDINC)/include -I$(LDDINC)

ifneq ($(KERNELRELEASE),)

# timer
obj-m := timer.o
obj-m += hrtimer.o
obj-m += delay_exec.o

# procfs
obj-m += procfs_demo1.o
obj-m += procfs_demo2.o

# notifer
obj-m += notifier.o

# debug
obj-m += kprobe_demo.o
obj-m += jprobe_demo.o

# Linux device & driver model
obj-m += kobject.o

else

VERSION = $(shell cat version)
KERNELDIR ?= /lib/modules/$(shell uname -r)/build
PWD       := $(shell pwd)

modules:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) LDDINC=$(PWD) modules

endif

install:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) LDDINC=$(PWD) modules_install

clean:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) LDDINC=$(PWD) clean
