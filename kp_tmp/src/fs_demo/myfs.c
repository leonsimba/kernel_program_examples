/*
 * myfs_demo1.c -- Sample demostration for filesystem
 *
 * Copyright (C) 2015 hezhiping
 * Version : Linux-2.6.32
 *
 * The source code in this file can be freely used, adapted,
 * and redistributed in source or binary form, so long as an
 * acknowledgment appears in derived source files.
 */
#include <linux/module.h>
#include <linux/init.h>

#include <linux/fs>

static int __init init_my_fs(void)
{
	int err;

	err = register_filesystem(&my_fs_type);
	if (err < 0) {
		printk(KERN_ERR "Failed to register filesystem\n");
		return err;
	}
	return err;
}

static void __exit exit_v9fs(void)
{
	//v9fs_sysfs_cleanup();
	//v9fs_cache_unregister();
	unregister_filesystem(&v9fs_fs_type);
}

module_init(init_my_fs)
module_init(exit_my_fs)
MODULE_AUTHOR("HeZhiping");
MODULE_LICENSE("Dual BSD/GPL");
MODULE_DESCRIPTION("A simple filesytem example.");
