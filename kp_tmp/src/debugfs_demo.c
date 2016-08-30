/*
 * debugfs_demo.c --  Simple demonstration of debugfs API.
 * 
 * Create the following debugfs:
 *     mydebug
 *        |-- a_u8
 *        |-- b_blob
 *        |-- subdir
 *            |-- c_reg  
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
#include <asm/uaccess.h>
#include <linux/debugfs.h>

#define BUF_SIZE    32
static u8 value_a = 0;
static char hello[BUF_SIZE] = "Hello world!\n";
static struct debugfs_blob_wrapper value_b;
static struct dentry *debugfs_demo_root;

static int filec_open(struct inode *inode, struct file *filp)
{
	filp->private_data = inode->i_private;
	return 0;
}

static ssize_t 
filec_read(struct file *filp, char __user *buffer, size_t count, loff_t *ppos)
{
	if (*ppos >= BUF_SIZE)
		return 0;
	if (*ppos + count > BUF_SIZE)
		count = BUF_SIZE - *ppos;

	if (copy_to_user(buffer, hello + *ppos, count))
		return -EFAULT;

	*ppos += count;

	return count;
}

static ssize_t 
filec_write(struct file *filp, const char __user *buffer, size_t count, loff_t *ppos)
{
	if (*ppos >= BUF_SIZE)
		return 0;
	if (*ppos + count > BUF_SIZE)
		count = BUF_SIZE - *ppos;

	if (copy_from_user(hello + *ppos, buffer, count))
		return -EFAULT;

	*ppos += count;

	return count;
}

struct file_operations file_c_fops = {
	.owner = THIS_MODULE,
	.open  = filec_open,
	.read  = filec_read,
	.write = filec_write,
};

static int __init debugfs_demo_init(void)
{
	struct dentry *sub_dir, *file_a, *file_b, *file_c;

	debugfs_demo_root = debugfs_create_dir("mydebug", NULL);
	if (!debugfs_demo_root)
		return -ENODEV;

	file_a = debugfs_create_x8("a_u8", 0644, debugfs_demo_root, &value_a);
	if (!file_a)
		goto Fail;

	value_b.data = (void *)hello;
	value_b.size = strlen(hello) + 1;
	file_b = debugfs_create_blob("b_blob", 0644, debugfs_demo_root, &value_b);
	if (!file_b)
		goto Fail;

	sub_dir = debugfs_create_dir("subdir", debugfs_demo_root);
	if (!sub_dir)
		goto Fail;

	file_c = debugfs_create_file("c_reg", 0644, sub_dir, NULL, &file_c_fops);
	if (!file_c)
		goto Fail;

	return 0;

Fail:
	debugfs_remove_recursive(debugfs_demo_root);
	debugfs_demo_root = NULL;
	return -ENODEV;
}

static void __exit debugfs_demo_exit(void)
{
	if (debugfs_demo_root)
		debugfs_remove_recursive(debugfs_demo_root);
        return;
}

module_init(debugfs_demo_init);
module_exit(debugfs_demo_exit);
MODULE_AUTHOR("HeZhiping");
MODULE_LICENSE("Dual BSD/GPL");
MODULE_DESCRIPTION("A simple demo to show how to use debugfs APIs");
