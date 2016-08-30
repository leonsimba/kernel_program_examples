/*
 * seqfile_demo.c -- Simple demonstration of the seq_file interface.
 *
 * Copyright (C) 2015 hezhiping
 * Version : Linux-2.6.32
 *
 * The source code in this file can be freely used, adapted,
 * and redistributed in source or binary form, so long as an
 * acknowledgment appears in derived source files.
 */

#include <linux/init.h>
#include <linux/module.h>

#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/errno.h>

#define DATA_NUM	10
struct my_data {
	unsigned v1;
	unsigned v2;
};
static struct my_data *data;

/*
 * The sequence iterator functions.  The position as seen by the
 * filesystem is just the count that we return.
 */
static void *seqfile_start(struct seq_file *s, loff_t *pos)
{
	if (*pos >= DATA_NUM)
		return NULL;
	return data + *pos;
}

static void *seqfile_next(struct seq_file *s, void *v, loff_t *pos)
{
	(*pos)++;
	if (*pos >= DATA_NUM)
		return NULL;
	return data + *pos;
}

static int seqfile_show(struct seq_file *s, void *v)
{
	struct my_data *d = (struct my_data *) v;

	seq_printf(s, "%u %u\n", d->v1, d->v2);
	return 0;
}

/*
 * The stop() function is called when iteration is complete;
 * Its job, of course, is to clean up.
 * If dynamic memory is allocated for the iterator, stop() is the place to free it.
 */
static void seqfile_stop(struct seq_file *s, void *v)
{
	/* Actually, there's nothing to do here */
}

/*
 * Tie them all together into a set of seq_operations.
 */
static struct seq_operations seqfile_ops = {
	.start = seqfile_start,
	.next  = seqfile_next,
	.stop  = seqfile_stop,
	.show  = seqfile_show
};


/*
 * Time to set up the file operations for our /proc file.  In this case,
 * all we need is an open function which sets up the sequence ops.
 */
static int ct_open(struct inode *inode, struct file *file)
{
	return seq_open(file, &seqfile_ops);
};

/*
 * The file operations structure contains our open function along with
 * set of the canned seq_ ops.
 */
static struct file_operations ct_file_ops = {
	.owner   = THIS_MODULE,
	.open    = ct_open,
	.read    = seq_read,
	.llseek  = seq_lseek,
	.release = seq_release
};

static int __init seqfile_demo_init(void)
{
	struct proc_dir_entry *entry;
	int i;

	data = kmalloc(sizeof(struct my_data) * DATA_NUM, GFP_KERNEL);
	if (!data)
		return -ENOMEM;
	for (i = 0; i < DATA_NUM; i++) {
		data[i].v1 = i;
		data[i].v2 = i + 100;
	}

	/* create a proc file - /proc/sequence */
	entry = create_proc_entry("sequence", 0, NULL);
	if (entry)
		entry->proc_fops = &ct_file_ops;
	return 0;
}

static void __exit seqfile_demo_exit(void)
{
	if (!data)
		kfree(data);
	remove_proc_entry("sequence", NULL);
}

module_init(seqfile_demo_init);
module_exit(seqfile_demo_exit);
MODULE_AUTHOR("HeZhiping");
MODULE_LICENSE("Dual BSD/GPL");
MODULE_DESCRIPTION("Simple demonstration of the seq_file interface.");
