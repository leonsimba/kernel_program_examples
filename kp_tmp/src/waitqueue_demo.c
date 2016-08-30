/*
 * waitqueue_demo.c -- the writers awake the readers
 *
 * Copyright (C) 2015 hezhiping
 * Version : Linux-2.6.32
 *
 * The source code in this file can be freely used, adapted,
 * and redistributed in source or binary form, so long as an
 * acknowledgment appears in derived source files.
 *
 * How to use:
 *     # insmod waitqueue_demo.ko
 *     # cat /proc/devices
 *     249  waitqueue
 *     # mknod /dev/waitqueue c 249 0
 */

#include <linux/module.h>
#include <linux/init.h>

#include <linux/fs.h>     /* everything... */
#include <linux/types.h>  /* size_t */
#include <linux/kernel.h> /* printk() */
#include <linux/sched.h>  /* current and everything */
#include <linux/wait.h>

static int major = 0;
static int flag = 0;
static DECLARE_WAIT_QUEUE_HEAD(wq);

ssize_t waitqueue_demo_read (struct file *filp, char __user *buf, size_t count, loff_t *pos)
{
	printk(KERN_DEBUG "process %i (%s) going to sleep\n", current->pid, current->comm);
	wait_event_interruptible(wq, flag != 0);
	flag = 0;
	printk(KERN_DEBUG "process %i (%s) is awoken \n", current->pid, current->comm);
	return 0; /* EOF */
}

ssize_t waitqueue_demo_write (struct file *filp, const char __user *buf, size_t count, loff_t *pos)
{
	printk(KERN_DEBUG "process %i (%s) awakening the readers...\n",
			current->pid, current->comm);
	flag = 1;
	wake_up_interruptible(&wq);
	/* succeed, to avoid retrial */
	return count;
}

struct file_operations waitqueue_demo_fops = {
	.owner = THIS_MODULE,
	.read =  waitqueue_demo_read,
	.write = waitqueue_demo_write,
};

static int __init waitqueue_demo_init(void)
{
	int result;

	/* Register your major, and accept a dynamic number */
	result = register_chrdev(major, "waitqueue_demo", &waitqueue_demo_fops);
	if (result < 0)
		return result;

	/* dynamic allocate major number*/
	if (major == 0)
		major = result; 
	return 0;
}

static void __exit waitqueue_demo_exit(void)
{
	unregister_chrdev(major, "waitqueue_demo");
}

module_init(waitqueue_demo_init);
module_exit(waitqueue_demo_exit);
MODULE_AUTHOR("HeZhiping");
MODULE_LICENSE("Dual BSD/GPL");
MODULE_DESCRIPTION("Simple demonstration of the seq_file interface.");
