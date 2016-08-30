/*
 * timer_demo.c --  An example to show timer_list's APIs.
 *
 * Copyright (C) 2015 hezhiping
 * Version : Linux-2.6.32
 *
 * The source code in this file can be freely used, adapted,
 * and redistributed in source or binary form, so long as an
 * acknowledgment appears in derived source files.
 *
 */

#include <linux/kernel.h>	/* printk() */
#include <linux/module.h>

#include <linux/slab.h>		/* for kmalloc() */
#include <linux/jiffies.h>      /* for global var jiffies and HZ */
#include <linux/errno.h>	/* error codes */
#include <linux/timer.h>        /* for xxx_timer() */

struct my_data {
	unsigned v1;
	unsigned v2;
};
static struct my_data *data;
static struct timer_list timer;

static void timer_demo_timerout(unsigned long data)
{
	struct my_data *dptr = (struct my_data *)data;

	dptr->v1 += 1;
	dptr->v2 += 1;
	printk("In timer, v1=%d, v2=%d\n", dptr->v1, dptr->v2);

	/* Step3. motify the timer
	 * int mod_timer(struct timer_list *timer, unsigned long expires)
	 */
	mod_timer(&timer, jiffies + 3 * HZ);
}

static int __init timer_demo_init_module(void)
{
	data = kmalloc(sizeof(struct my_data), GFP_KERNEL);
	if (!data) {
		printk("kmalloc failed\n");
		return -ENOMEM;
	}

	data->v1 = 0;
	data->v2 = 1;

	/* Step1. initial a timer
	 * void init_timer(struct timer_list * timer)
	 */
	init_timer(&timer);
	timer.data = (unsigned long)data;
	timer.function = timer_demo_timerout;
	timer.expires = jiffies + 2 * HZ;

	/* Step2. active the timer
	 * void add_timer(struct timer_list *timer)
	 */
	add_timer(&timer);

	return 0;
}

static void __exit timer_demo_exit_module(void)
{
	int ret;

	/* Step4. delete the timer
	 * int del_timer_sync(struct timer_list * timer)
	 */
	ret = del_timer_sync(&timer);
	if (ret == 0) {
		printk("Timer was already deactived,"
				"before call del_timer_sync()");
	}

	if (data)
		kfree(data);
}

module_init(timer_demo_init_module);
module_exit(timer_demo_exit_module);
MODULE_AUTHOR("HeZhiping");
MODULE_LICENSE("Dual BSD/GPL");
MODULE_DESCRIPTION("A simple demo to show how to use timer_list");
