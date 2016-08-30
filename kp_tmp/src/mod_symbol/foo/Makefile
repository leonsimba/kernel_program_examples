#KBUILD_EXTRA_SYMBOLS=/home/mod/bar/Module.symvers
#KBUILD_EXTRA_SYMBOLS=//home/kernel_program/kernel/demo/mod_symbol/foo/../bar/Module.symvers
obj-m := foo.o
all:
	echo ${KBUILD_EXTRA_SYMBOLS}
	make -C /lib/modules/`uname -r`/build M=`pwd` KBUILD_EXTRA_SYMBOLS=/`pwd`/../bar/Module.symvers modules
clean:
	make -C /lib/modules/`uname -r`/build M=`pwd` clean
