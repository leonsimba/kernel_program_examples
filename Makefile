SUBDIRS = src
DATE = `date +%y_%m_%d_%H_%M_%S`

.PHONY:all clean install
all:
	for i in $(SUBDIRS); do 	\
		make -C $$i;		\
	done

clean:
	for i in $(SUBDIRS); do		\
		make -C $$i clean;	\
	done

install:
	for i in $(SUBDIRS); do	\
		make -C $$i install;	\
	done

