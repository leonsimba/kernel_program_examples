/*
 * proc_demo1.c --  An example to show how to use procfs API.
 *                  create /proc/procfs_demo1/fortune
 *
 * Copyright (C) 2015 hezhiping
 * Version : Linux-2.6.32
 *
 * The source code in this file can be freely used, adapted,
 * and redistributed in source or binary form, so long as an
 * acknowledgment appears in derived source files.
 */
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/string.h>
#include <linux/vmalloc.h>
#include <asm/uaccess.h>

#define MAX_COOKIE_LENGTH    PAGE_SIZE
static struct proc_dir_entry *proc_dir;

static char *cookie_pot;  // Space for fortune strings
static int cookie_index;  // Index to write next fortune
static int next_fortune;  // Index to read next fortune

static int fortune_write(struct file *filp, const char __user *buff,
			unsigned long len, void *data)
{
	int space_available = (MAX_COOKIE_LENGTH-cookie_index)+1;

	if (len > space_available) {
		printk(KERN_INFO "fortune: cookie pot is full!\n");
		return -ENOSPC;
	}

	if (copy_from_user( &cookie_pot[cookie_index], buff, len )) {
		return -EFAULT;
	}
	cookie_index += len;
	cookie_pot[cookie_index-1] = 0;
	return len;
}

static int fortune_read(char *page, char **start, off_t off,
			int count, int *eof, void *data)
{
	int len;
	if (off > 0) {
		*eof = 1;
		return 0;
	}
	/* Wrap-around */
	if (next_fortune >= cookie_index)
		next_fortune = 0;

	len = sprintf(page, "%s\n", &cookie_pot[next_fortune]);
	next_fortune += len;
	return len;
}

static int procfs_demo1_init(void)
{
	int ret = 0;
	struct proc_dir_entry *proc_entry;

	cookie_pot = (char *)vmalloc(MAX_COOKIE_LENGTH);
	if (!cookie_pot) {
		ret = -ENOMEM;
		return ret;
	}
	memset(cookie_pot, 0, MAX_COOKIE_LENGTH);
	cookie_index = 0;
	next_fortune = 0;

	/* create directory /proc/procfs_demo1 */
	proc_dir = proc_mkdir("procfs_demo1", NULL);
	if(proc_dir == NULL) {
		ret = -ENOMEM;
		printk(KERN_INFO "[procfs_demo1]: proc_mkdir failed\n");
		goto err;
	}

	/* create file /proc/procfs_demo1/fortune */
	proc_entry = create_proc_entry("fortune", 0644, proc_dir);
	if (proc_entry == NULL) {
		ret = -ENOMEM;
		printk(KERN_INFO "[procfs_demo1]: create_proc_entry failed\n");
		goto err;
	}

	proc_entry->read_proc = fortune_read;
	proc_entry->write_proc = fortune_write;
	return ret;
err:
	vfree(cookie_pot);
	return ret;
}

static void procfs_demo1_exit(void)
{
	remove_proc_entry("fortune", proc_dir);
	remove_proc_entry("procfs_demo1", NULL);
	vfree(cookie_pot);
}

module_init(procfs_demo1_init);
module_exit(procfs_demo1_exit);
MODULE_AUTHOR("HeZhiping");
MODULE_LICENSE("Dual BSD/GPL");
MODULE_DESCRIPTION("An example to show how to use procfs APIs");
