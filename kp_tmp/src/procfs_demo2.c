/*
 * proc_demo2.c --  another demonstration of profs API.
 *                  use the file_operation way to manage the file.

 * Copyright (C) 2015 hezhiping
 * Version : Linux-2.6.32
 *
 * The source code in this file can be freely used, adapted,
 * and redistributed in source or binary form, so long as an
 * acknowledgment appears in derived source files.
 *
 */
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/proc_fs.h>      // for proc fs APIs
#include <asm/uaccess.h>        // for copy_xxx_user
#include <linux/cred.h>         // for get_current_cred()
#include <linux/sched.h>

#define PROC_ENTRY_FILENAME     "buffer2k"
#define PROCFS_MAX_SIZE         2048

static char procfs_buffer[PROCFS_MAX_SIZE];  //The buffer (2k) for this module
static unsigned long procfs_buffer_size = 0; //The size of the data hold in the buffer

static ssize_t procfs_read(struct file *filp, char *buffer,
			size_t length, loff_t * offset)
{
	static int finished = 0;

	if (finished) {
		printk(KERN_INFO "procfs_read: END\n");
		finished = 0;
		return 0;
	}
	finished = 1;
	if (copy_to_user(buffer, procfs_buffer, procfs_buffer_size))
		return -EFAULT;

	printk(KERN_INFO "procfs_read: read %lu bytes\n", procfs_buffer_size);
	/* Return the number of bytes "read" */
	return procfs_buffer_size;
}

static ssize_t procfs_write(struct file *file, const char *buffer,
			size_t len, loff_t * off)
{
	if (len > PROCFS_MAX_SIZE) {
		procfs_buffer_size = PROCFS_MAX_SIZE;
	} else {
		procfs_buffer_size = len;
	}

	if (copy_from_user(procfs_buffer, buffer, procfs_buffer_size))
		return -EFAULT;

	printk(KERN_INFO "procfs_write: write %lu bytes\n", procfs_buffer_size);
	return procfs_buffer_size;
}

/*
 * This function decides whether to allow an operation
 *
 * The operation can be one of the following values:
 *      0 - Execute (run the "file" - meaningless in our case)
 *      2 - Write (input to the kernel module)
 *      4 - Read (output from the kernel module)
 *
 * Return:
 * 	0            allowed
 * 	non-zero     not allowed
 */
static int module_permission(struct inode *inode, int op)
{
	int ret = -EACCES;
	const struct cred *cred;

	/* We allow everybody to read from our module, but
         * only root (uid 0) may write to it
         */
	if (op & MAY_READ)
		ret = 0;

	if (op & MAY_WRITE) {
		cred = get_current_cred();
		if (cred->euid == 0)
			ret = 0;
		else
			ret = -EACCES;
		put_cred(cred);
	}
	return ret;
}

static int procfs_open(struct inode *inode, struct file *file)
{       //increment the module's reference count.
	try_module_get(THIS_MODULE);
	return 0;
}

static int procfs_close(struct inode *inode, struct file *file)
{
	module_put(THIS_MODULE);
	return 0;
}

static struct file_operations my_procfile_fops = {
	.read     = procfs_read,
	.write    = procfs_write,
	.open     = procfs_open,
	.release  = procfs_close,
};

static struct inode_operations my_procfile_iops = {
	.permission = module_permission,
};

static int __init proc_demo2_init(void)
{
	struct proc_dir_entry *my_procfile;

	/* create file /proc/PROC_ENTRY_FILENAME */
	my_procfile = create_proc_entry(PROC_ENTRY_FILENAME, 0644, NULL);
	if (my_procfile == NULL){
		printk(KERN_ALERT "[procfs_demo2] create /proc/%s failed\n",
			PROC_ENTRY_FILENAME);
		return -ENOMEM;
	}
	my_procfile->proc_iops = &my_procfile_iops;
	my_procfile->proc_fops = &my_procfile_fops;
	my_procfile->mode = S_IFREG | S_IRUGO | S_IWUSR;
	my_procfile->uid = 0;
	my_procfile->gid = 0;
	my_procfile->size = 80;

	printk(KERN_INFO "[procfs_demo2] /proc/%s was created\n",
			PROC_ENTRY_FILENAME);
	return 0;
}

static void __exit proc_demo2_exit(void)
{
	remove_proc_entry(PROC_ENTRY_FILENAME, NULL);
	printk(KERN_INFO "[procfs_demo2] /proc/%s was removed\n",
			PROC_ENTRY_FILENAME);
}

module_init(proc_demo2_init);
module_exit(proc_demo2_exit);
MODULE_AUTHOR("HeZhiping");
MODULE_LICENSE("Dual BSD/GPL");
MODULE_DESCRIPTION("An another simple demo to show how to use procfs APIs");
