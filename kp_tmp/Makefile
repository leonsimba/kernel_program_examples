SUBDIR = src tests tools
DATE = `date +%y_%m_%d_%H_%M_%S`

all:
	for dir in $(SUBDIR); do
		make -C $(dir)
	done

clean:
	for dir in $(SUBDIR); do
		make -C $(dir) clean
	done

install:
	for dir in $(SUBDIR); do
		make -C $(dir) install
	done

backup: clean
	tar zcvf $(NDF_TRUNK)/../srcbackup/ndf-src-$(DATE).tgz $(NDF_TRUNK)

.PHONY:all config clean install tags
