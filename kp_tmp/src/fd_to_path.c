/*
 * vfs_file.c -- get pathname by fd 
 *
 * Copyright (C) 2015 hezhiping
 * Version : Linux-2.6.32
 *
 * The source code in this file can be freely used, adapted,
 * and redistributed in source or binary form, so long as an
 * acknowledgment appears in derived source files.
 */
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/moduleparam.h>

#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/string.h>
#include <linux/fdtable.h>
#include <linux/sched.h>

static uint pid = 1;
module_param(pid, uint, S_IRUGO);

static uint fd = 1;
module_param(fd, uint, S_IRUGO);

static struct task_struct * get_task_struct_by_pid(pid_t pid)
{
	struct task_struct *task = NULL;
	struct pid *pid_struct = NULL;

	pid_struct = find_get_pid(pid);
	if (!pid_struct)
		return NULL;

	task = get_pid_task(pid_struct, PIDTYPE_PID);
	if (!task)
		return NULL;

	put_pid(pid_struct);
	return task;
}

static int get_file_pathname(struct task_struct *task, int fd)
{
	struct file *file = NULL;
	struct files_struct *files = NULL;
	char *p, pathname[256];
	int ret = -ENOENT;	

	rcu_read_lock();
	files = task->files;
	if (!files) {
		printk("files is null.\n");
		goto err;
	}

	file = files->fdt->fd[fd];
	if (!file) {
		printk("file is null.\n");
		goto err;
	}

	p = d_path(&(file->f_path), pathname, sizeof(pathname));
	if (IS_ERR(p)) {
		ret = PTR_ERR(p);
		printk("d_path failed, err = %d\n", ret);
		goto err;
	} else {
		int size;
		size = strlen(p);	
		memmove(pathname, p, size);
		pathname[size] = '\n';         // Add a newline
		pathname[++size] = 0;
	}

	printk("fd = %d, file path = %s\n", fd, pathname);

	return 0;
err:
	rcu_read_unlock();
	return ret;
}

static int __init path_demo_init(void)
{
	int ret = 0;
	struct task_struct *task;

	task = get_task_struct_by_pid(pid);
	if (!task)
		return -1;

	ret = get_file_pathname(task, fd);
	put_task_struct(task);

	return ret;
}

static void __exit path_demo_exit(void)
{
	return;
}

module_init(path_demo_init);
module_exit(path_demo_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("HeZhiping <hezhi11@126.com>");
