/*
 * sysfs_demo1.c -- Sample demostration fo sysfs
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

#include <linux/types.h>	/* for strtoul() */
#include <linux/sysfs.h>	/* for sysfs_xxx_file() */
#include <linux/kobject.h>

static unsigned long val = 0;

static ssize_t myfile_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	return sprintf(buf, "%lu\n", val);;
}

static ssize_t myfile_store(struct kobject *kobj, struct kobj_attribute *attr,
			const char *buf, size_t n)
{
	int ret;

	ret = strict_strtoul(buf, 10, &val);
	if (ret < 0)
		return ret;
	return n;
}
/*
 * struct kobj_attribute {                                                                                                                                      
 *        struct attribute attr;
 *        ssize_t (*show)(struct kobject *kobj, struct kobj_attribute *attr, char *buf);
 *        ssize_t (*store)(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count);
 * };
 *
 * #define __ATTR(_name,_mode,_show,_store) { \
 *        .attr = {.name = __stringify(_name), .mode = _mode },   \
 *	  .show   = _show,                                        \
 *        .store  = _store,                                       \                                                                                            
 *       }
 * */
static struct kobj_attribute my_attr = 
	__ATTR(myfile, S_IRUSR | S_IWUSR, myfile_show, myfile_store);

static int __init sysfs_demo1_init(void)
{
	/* create a sysfile /sys/kerenl/myfile */
	return sysfs_create_file(kernel_kobj, &my_attr.attr);
}

static void __exit sysfs_demo1_exit(void)
{
	return sysfs_remove_file(kernel_kobj, &my_attr.attr);
}

module_init(sysfs_demo1_init);
module_exit(sysfs_demo1_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("HeZhiping <hezhi11@126.com>");
MODULE_DESCRIPTION("A simple demo to show how to use sysfs APIs");
