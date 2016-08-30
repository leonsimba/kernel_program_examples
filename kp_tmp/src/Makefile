# Comment/uncomment the following line to enable/disable debugging
#DEBUG = y

#DEBUG_FNAT = y

ifeq ($(DEBUG),y)
  DEBFLAGS = -O -g # "-O" is needed to expand inlines
else
  DEBFLAGS = -O2
endif

ifeq ($(UNDER_DEV), y)
  DEBFLAGS += -DUNDER_DEV
endif

EXTRA_CFLAGS += $(DEBFLAGS)
EXTRA_CFLAGS += -I$(LDDINC)/include -I$(LDDINC)

ifneq ($(KERNELRELEASE),)

obj-m := timer.o
obj-m += hrtimer_demo.o
obj-m += curtime_demo.o
obj-m += delay_exec_demo.o

obj-m += seqfile_demo.o
obj-m += procfs_demo1.o
obj-m += procfs_demo2.o
obj-m += sysctl_demo.o
obj-m += debugfs_demo.o
obj-m += file_demo.o
obj-m += fd_to_path.o

obj-m += notifier_demo.o
obj-m += notifier_netdevice_demo.o

obj-m += mod_param_demo.o

#obj-m += kset_demo.o
#obj-m += kobject_demo.o
obj-m += sysfs_demo1.o

obj-m += waitqueue_demo.o
obj-m += kernel_thread_demo.o
obj-m += kthread_create_demo.o
obj-m += eventfd_demo.o

# kernel data struct demo
obj-m += idr_demo.o
obj-m += htable_fix_length.o

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
