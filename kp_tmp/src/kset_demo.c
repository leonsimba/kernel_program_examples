/*
 * kset_demo.c -- This module shows how to create a kset in sysfs called
 *                /sys/kernel/kset-example
 *
 * Then tree kobjects are created and assigned to this kset, "foo", "baz",
 * and "bar".  In those kobjects, attributes of the same name are also
 * created and if an integer is written to these files, it can be later
 * read out of it.
 *
 * Copyright (C) 2015 hezhiping
 * Version : Linux-2.6.32
 *
 * The source code in this file can be freely used, adapted,
 * and redistributed in source or binary form, so long as an
 * acknowledgment appears in derived source files.
 */
#include <linux/kobject.h>
#include <linux/string.h>
#include <linux/sysfs.h>
#include <linux/module.h>
#include <linux/init.h>

/*
 * This is our "object" that we will create a few of and register them with
 * sysfs.
 */
struct demo_obj {
	struct kobject kobj;
	int foo;
	int baz;
	int bar;
};
#define to_demo_obj(x) container_of(x, struct demo_obj, kobj)

/* a custom attribute that works just for a struct demo_obj. */
struct demo_attribute {
	struct attribute attr;
	ssize_t (*show)(struct demo_obj *foo, struct demo_attribute *attr, char *buf);
	ssize_t (*store)(struct demo_obj *foo, struct demo_attribute *attr, const char *buf, size_t count);
};
#define to_foo_attr(x) container_of(x, struct demo_attribute, attr)



/*
 * More complex function where we determine which varible is being accessed by
 * looking at the attribute for the "baz" and "bar" files.
 */
static ssize_t 
a_show(struct demo_obj *demo_obj, struct demo_attribute *attr,
		      char *buf)
{
	int var;

	if (strcmp(attr->attr.name, "foo") == 0)
		var = demo_obj->foo;
	else if (strcmp(attr->attr.name, "bar") == 0) 
		var = demo_obj->bar;
	else
		var = demo_obj->baz;
	return sprintf(buf, "%d\n", var);
}

static ssize_t 
a_store(struct demo_obj *demo_obj, struct demo_attribute *attr, const char *buf, size_t count)
{
	int var;

	sscanf(buf, "%du", &var);
	if (strcmp(attr->attr.name, "foo") == 0)
		demo_obj->foo = var;
	else if (strcmp(attr->attr.name, "baz") == 0) 
		demo_obj->baz = var;
	else
		demo_obj->bar = var;
	return count;
}


static struct demo_attribute foo_attribute = __ATTR(foo, 0666, a_show, a_store);
static struct demo_attribute baz_attribute = __ATTR(baz, 0666, a_show, a_store);
static struct demo_attribute bar_attribute = __ATTR(bar, 0666, a_show, a_store);

static struct attribute *foo_default_attrs[] = {
	&foo_attribute.attr,
	&baz_attribute.attr,
	&bar_attribute.attr,
	NULL,
};

/*
 * The release function for our object.  This is REQUIRED by the kernel to
 * have.  We free the memory held in our object here.
 *
 * NEVER try to get away with just a "blank" release function to try to be
 * smarter than the kernel.  Turns out, no one ever is...
 */
static void foo_release(struct kobject *kobj)
{
	struct demo_obj *foo;

	foo = to_demo_obj(kobj);
	kfree(foo);
}

/* when read sysfs attribute file, this function will be called */
static ssize_t
foo_attr_show(struct kobject *kobj, struct attribute *attr, char *buf)
{
	struct demo_attribute *attribute;
	struct demo_obj *foo;

	attribute = to_foo_attr(attr);
	foo = to_demo_obj(kobj);

	if (!attribute->show)
		return -EIO;

	return attribute->show(foo, attribute, buf);
}

/* when write sysfs attribute file, this function will be called */
static ssize_t
foo_attr_store(struct kobject *kobj, struct attribute *attr, const char *buf, size_t len)
{
	struct demo_attribute *attribute;
	struct demo_obj *foo;

	attribute = to_foo_attr(attr);
	foo = to_demo_obj(kobj);

	if (!attribute->store)
		return -EIO;

	return attribute->store(foo, attribute, buf, len);
}

/* Our custom sysfs_ops that we will associate with our ktype later on */
static struct sysfs_ops foo_sysfs_ops = {
	.show  = foo_attr_show,
	.store = foo_attr_store,
};

/*
 * Our own ktype for our kobjects.  Here we specify our sysfs ops, the
 * release function, and the set of default attributes we want created
 * whenever a kobject of this type is registered with the kernel.
 */
static struct kobj_type foo_ktype = {
	.sysfs_ops     = &foo_sysfs_ops,
	.release       = foo_release,
	.default_attrs = foo_default_attrs,
};

static struct kset *kset_demo;
static struct demo_obj *demo_obj;
static struct demo_obj *bar_obj;
static struct demo_obj *baz_obj;

static struct demo_obj *create_demo_obj(const char *name, const struct kset *kset)
{
	struct demo_obj *foo;
	int retval;

	// allocate the memory for the whole object
	if (!foo)
		return NULL;

	/*
	 * As we have a kset for this kobject, we need to set it before calling
	 * the kobject core.
	 */
	foo->kobj.kset = kset;

	/*
	 * Initialize and add the kobject to the kernel.  All the default files
	 * will be created here.  As we have already specified a kset for this
	 * kobject, we don't have to set a parent for the kobject, the kobject
	 * will be placed beneath that kset automatically.
	 */
	retval = kobject_init_and_add(&foo->kobj, &foo_ktype, NULL, "%s", name);
	if (retval) {
		kobject_put(&foo->kobj);
		return NULL;
	}

	// send an uevent to userspace that the kobject was added
	kobject_uevent(&foo->kobj, KOBJ_ADD);

	return foo;
}

static void destroy_demo_obj(struct demo_obj *foo)
{
	kobject_put(&foo->kobj);
}

static int __init kset_demo_init(void)
{
	// Create a kset with the name of "kset_demo", located under /sys/kernel/
	kset_demo = kset_create_and_add("kset_demo", NULL, kernel_kobj);
	if (!kset_demo)
		return -ENOMEM;

	// Create three objects and register them with our kset
	demo_obj = create_demo_obj("foo", kset_demo);
	if (!demo_obj)
		goto foo_error;

	bar_obj = create_demo_obj("bar", kset_demo);
	if (!bar_obj)
		goto bar_error;

	baz_obj = create_demo_obj("baz", kset_demo);
	if (!baz_obj)
		goto baz_error;

	return 0;

baz_error:
	destroy_demo_obj(bar_obj);
bar_error:
	destroy_demo_obj(demo_obj);
foo_error:
	return -EINVAL;
}

static void __exit kset_demo_exit(void)
{
	destroy_demo_obj(baz_obj);
	destroy_demo_obj(bar_obj);
	destroy_demo_obj(demo_obj);
	kset_unregister(kset_demo);
}

module_init(kset_demo_init);
module_exit(kset_demo_exit);
MODULE_AUTHOR("HeZhiping");
MODULE_LICENSE("Dual BSD/GPL");
MODULE_DESCRIPTION("A simple demo to show how to use kset APIs");
