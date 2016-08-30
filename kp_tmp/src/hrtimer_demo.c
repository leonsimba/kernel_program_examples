/*
 * hrtimer_demo.c -- show how to user high resolution timer
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
#include <linux/moduleparam.h>

#include <linux/errno.h>	/* error codes */
#include <linux/jiffies.h>      /* for global var jiffies and HZ */
#include <linux/hrtimer.h>      /* for xxx_timer() */

static int restart = 0;
module_param(restart, int, S_IRUSR | S_IWUSR);

static struct hrtimer timer;
ktime_t kt;

static enum hrtimer_restart
hrtimer_demo_handle(struct hrtimer *timer)
{
	printk("In hrtimer_demo__handle()\n");

	/* 3. motify the timer */
	hrtimer_forward(timer, timer->base->get_time(), kt);

	if (restart)
		return HRTIMER_RESTART;
	else
		return HRTIMER_NORESTART;

}

static int __init timer_demo_init_module(void)
{
	/* 1sec, 10 nsec */
	kt = ktime_set(1, 10);

	/* 1. initial the timer */
	hrtimer_init(&timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);

	/* 2. active the timer */
	timer.function = hrtimer_demo_handle;
	hrtimer_start(&timer, kt, HRTIMER_MODE_REL);

	return 0;
}

static void __exit timer_demo_exit_module(void)
{
	/* 4. cancel the timer */
	hrtimer_cancel(&timer);
}

module_init(timer_demo_init_module);
module_exit(timer_demo_exit_module);
MODULE_AUTHOR("HeZhiping");
MODULE_LICENSE("Dual BSD/GPL");
MODULE_DESCRIPTION("A simple demo to show how to use hrtimer");
