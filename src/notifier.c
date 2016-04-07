/*
 * notifier.c -- An example of user-defined notifier.
 *
 * Copyright (C) 2015 ZhiPing.He
 * Version : Linux-2.6.32
 *
 * The source code in this file can be freely used, adapted,
 * and redistributed in source or binary form, so long as an
 * acknowledgment appears in derived source files.
 *
 * Author:
 *      ZhiPing.He <zhiping.hzp@alibaba-inc.com>
 */
#include <linux/module.h>
#include <linux/kernel.h>

#include <linux/notifier.h>

enum {
	MY_EVENT1,
	MY_EVENT2,
};

struct my_data {
	int foo;
	int bar;
};

static int my_event_handler1(struct notifier_block *self,
				unsigned long val, void *data)
{
	printk("I'm notifer 1, event = %ld\n", val);
	return NOTIFY_DONE;
}

static int my_event_handler2(struct notifier_block *self,
				unsigned long val, void *data)
{
	struct my_data *ptr = (struct my_data *) data;

	printk("I'm notifer 2\n");
	switch (val) {
	case MY_EVENT1:
		printk("handle event1, foo = %d\n", ++(ptr->foo));
		break;
	case MY_EVENT2:
		printk("handle event2, bar = %d\n", ++(ptr->bar));
		break;
	default:
		break;
	}

	return NOTIFY_DONE;
}

/* User-defined notifier chain implementation */
static BLOCKING_NOTIFIER_HEAD(my_notifier_chain);

static struct notifier_block my_notifier1 = {
	.notifier_call = my_event_handler1,
	.priority = 1,
};

static struct notifier_block my_notifier2 = {
	.notifier_call = my_event_handler2,
	.priority = 2,
};

static int __init notifier_demo_init(void)
{
	/* Register two user-defined notifier to my_notifier_chain */
	blocking_notifier_chain_register(&my_notifier_chain, &my_notifier1);
	blocking_notifier_chain_register(&my_notifier_chain, &my_notifier2);
	return 0;
}

static void __exit notifier_demo_exit(void)
{
	struct my_data data;

	data.foo = 1;
	data.bar = 100;

	blocking_notifier_call_chain(&my_notifier_chain, MY_EVENT2, (void *)&data);

	blocking_notifier_chain_unregister(&my_notifier_chain, &my_notifier1);
	blocking_notifier_chain_unregister(&my_notifier_chain, &my_notifier2);
}

module_init(notifier_demo_init);
module_exit(notifier_demo_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("He ZhiPing <zhiping.hzp@alibaba-inc.com>");
MODULE_DESCRIPTION("An example of user-defined notifier");
