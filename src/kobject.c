/** kobject.c -- an sample to show how to use kobject's API
 *
 * This module shows how to create a simple subdirectory in sysfs
 * /sys/kernel/kobject_ex. In /sys/kernel/kobject_ex directory,
 * two files are created : "foo" and "bar".
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

#include <linux/slab.h>
#include <linux/kobject.h>
#include <linux/sysfs.h>

struct kobject_ex_dev {
	struct kobject *kobj;
	int foo;
	int	bar;
};
static struct kobject_ex_dev my_dev;
static struct kobj_type	my_type;

static ssize_t my_show(struct kobject *kobj, struct attribute *attr, char *buffer)
{
	struct kobject_ex_dev *dev;
	ssize_t count = 0;

	dev = container_of(kobj, struct kobject_ex_dev, kobj);

	if(strcmp(attr->name,"foo") == 0){
		count = sprintf(buffer,"%d\n",dev->foo);
	}else if(strcmp(attr->name,"bar") == 0){
		count = sprintf(buffer,"%d\n",dev->bar);
	}

	return count;
}

static ssize_t my_store(struct kobject *kobj, struct attribute *attr,
			const char *buffer, size_t count)
{
	struct kobject_ex_dev *dev;

	dev = container_of(kobj, struct kobject_ex_dev, kobj);

	if(strcmp(attr->name,"foo") == 0){
		sscanf(buffer,"%d", &dev->foo);
	}else if(strcmp(attr->name,"bar") == 0){
		sscanf(buffer,"%d", &dev->bar);
	}

	return count;
}

static void my_release(struct kobject * kobj)
{
	if(kobj)
		kfree(kobj);
}

static struct sysfs_ops my_sysfs_ops = {
	.show  = my_show,
	.store = my_store,
};

static struct attribute foo_attribute = {
	.name = "foo",
	.mode = 0666,
};

static struct attribute bar_attribute = {
	.name = "bar",
	.mode = 0666,
};

static struct attribute * my_attrs[] = {
	&foo_attribute,
	&bar_attribute,
	NULL,	/* need to NULL terminate the list of attributes */
};


static int __init kobject_ex_init(void)
{
	struct kobject *obj;
	int retval;

	obj = kzalloc(sizeof(struct kobject), GFP_KERNEL);
	if (obj == NULL) {
		printk(KERN_INFO "kzalloc failed");
		return -ENOMEM;
	}
	my_dev.kobj = obj;

	/* Initiale my kobj_type: my_type */
	my_type.default_attrs = my_attrs;
	my_type.sysfs_ops = &my_sysfs_ops;
	my_type.release = my_release;

	/* Initialize a kobject structure and add it to the kobject hierarchy */
	retval = kobject_init_and_add(my_dev.kobj, &my_type, kernel_kobj, "kobject_ex");

	return retval;
}

static void __exit kobject_ex_exit(void)
{
	kobject_del(my_dev.kobj);
	kobject_put(my_dev.kobj);
}

module_init(kobject_ex_init);
module_exit(kobject_ex_exit);
MODULE_AUTHOR("He Zhi ping");
MODULE_LICENSE("Dual BSD/GPL");
MODULE_DESCRIPTION("An simple example of kobject");
