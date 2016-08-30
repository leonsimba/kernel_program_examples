/*
 * eventfd_demo.c --  Simple demonstration of eventfd API.
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
#include <linux/err.h>
#include <linux/list.h>
#include <linux/eventfd.h>
#include <linux/spinlock.h>

static LIST_HEAD(event_list);
static DEFINE_SPINLOCK(event_lock);
struct event_item {
	struct eventfd_ctx *eventfd;
	struct list_head   list;
};

static ssize_t my_eventfd_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	struct list_head *cur, *next;
	unsigned int val = 0;

	spin_lock(&event_lock);
	list_for_each_safe(cur, next, &event_list) {
		struct event_item *e = container_of(cur, struct event_item, list);
		eventfd_signal(e->eventfd, 1);
		val++;
		list_del(cur);
		kfree(e);
	}
	spin_unlock(&event_lock);
	return sprintf(buf, "%u\n", val);;
}

static ssize_t my_eventfd_store(struct kobject *kobj, struct kobj_attribute *attr,
			const char *buf, size_t n)
{
	int ret;
	int fd;
	struct file *file;
	struct event_item *evt;

	evt = kmalloc(sizeof(struct event_item), GFP_KERNEL);
	if (!evt)
		return -ENOMEM;

        /* convert a string to an int */
	sscanf(buf, "%d", &fd);

	file = eventfd_fget(fd);
	if (IS_ERR(file)) {
		ret = PTR_ERR(file);
		goto out_free;
	}

	evt->eventfd = eventfd_ctx_fileget(file);
	if (IS_ERR(evt->eventfd)) {
		ret = PTR_ERR(evt->eventfd);
		goto out_fput;
	}

	INIT_LIST_HEAD(&evt->list);
	spin_lock(&event_lock);
	list_add(&evt->list, &event_list);
	spin_unlock(&event_lock);

	fput(file);
	return n;
out_fput:
	fput(file);
out_free:
	kfree(evt);
	return ret;
}

static struct kobj_attribute my_attr = 
	__ATTR(my_eventfd, S_IRUSR | S_IWUSR, my_eventfd_show, my_eventfd_store);

static int __init eventfd_demo_init(void)
{
	/* create a sysfile /sys/kerenl/myfile */
	return sysfs_create_file(kernel_kobj, &my_attr.attr);
}

static void __exit eventfd_demo_exit(void)
{
	struct list_head *cur, *next;

	list_for_each_safe(cur, next, &event_list) {
		struct event_item *e = container_of(cur, struct event_item, list);
		eventfd_signal(e->eventfd, 1);
		list_del(cur);
		kfree(e);
	}

	return sysfs_remove_file(kernel_kobj, &my_attr.attr);
}

module_init(eventfd_demo_init);
module_exit(eventfd_demo_exit);
MODULE_AUTHOR("HeZhiping");
MODULE_LICENSE("Dual BSD/GPL");
MODULE_DESCRIPTION("A simple demo to show how to use eventfd APIs");
