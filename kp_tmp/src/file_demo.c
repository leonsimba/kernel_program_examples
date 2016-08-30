/*
 * file_demo.c -- the file operation interfaces in kernel
 *
 * Copyright (C) 2015 hezhiping
 * Version : Linux-2.6.32
 *
 * The source code in this file can be freely used, adapted,
 * and redistributed in source or binary form, so long as an
 * acknowledgment appears in derived source files.
 */
#include <linux/kernel.h>
#include <linux/module.h>

#include <linux/fs.h>
#include <linux/uaccess.h>

static char buf[] = "hello world";
static char buf1[32];

static int __init file_demo_init(void)
{
	struct file *filp;
	mm_segment_t old_fs;  
	loff_t pos = 0;  
	int ret = 0;

	filp = filp_open("/tmp/kernel_file", O_RDWR | O_CREAT, 0644);  
	if (IS_ERR(filp)) {  
		ret = PTR_ERR(filp); 
		printk("filp_open() failed, err = %d\n", ret);  
		return ret;  
	}  

	old_fs = get_fs();  
	set_fs(KERNEL_DS);  
	vfs_write(filp, buf, sizeof(buf), &pos);  

	pos = 0;  
	memset(buf1, 0, sizeof(buf1));
	vfs_read(filp, buf1, sizeof(buf), &pos);  
	printk("read: %s\n", buf1);  

	set_fs(old_fs);  
	filp_close(filp, NULL);  
	return 0;  
}

static void __exit file_demo_exit(void)
{
	return;
}

module_init(file_demo_init);
module_exit(file_demo_exit);
MODULE_AUTHOR("HeZhiping");
MODULE_LICENSE("Dual BSD/GPL");
MODULE_DESCRIPTION("Operate file in kernel.");
