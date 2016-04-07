/*
 * hrtimer_demo.c -- An example to show how to use high resolution timer
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
#include <linux/jiffies.h>	/* for global var jiffies and HZ */
#include <linux/hrtimer.h>	/* for hrtimer_xxx() */

static int restart = 0;
module_param(restart, int, S_IRUSR | S_IWUSR);

static struct hrtimer timer;
ktime_t kt;

static enum hrtimer_restart
hrtimer_demo_handle(struct hrtimer *timer)
{
	printk("[hrtimer demo] In hrtimer_demo__handle\n");

	/* Step3. if now > expire, delay the timer's expire time.
         * timer->base->get_time() is used to get current time.
	 * u64 hrtimer_forward(struct hrtimer *timer, ktime_t now,
	 *		ktime_t interval)
	 */
	hrtimer_forward(timer, timer->base->get_time(), kt);

	if (restart)
		return HRTIMER_RESTART;
	else
		return HRTIMER_NORESTART;

}

static int __init hrtimer_demo_init_module(void)
{
	/* Expire time: 1sec, 10 nsec */
	kt = ktime_set(1, 10);

	/* Step1. initial a high timer
	 * void hrtimer_init(struct hrtimer *timer, clockid_t which_clock,
	 *		enum hrtimer_mode mode)
	 */
	hrtimer_init(&timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);

	/* Step2. active the high timer
	 * int hrtimer_start(struct hrtimer *timer, ktime_t time,
	 *		const enum hrtimer_mode mode)
	 */
	timer.function = hrtimer_demo_handle;
	hrtimer_start(&timer, kt, HRTIMER_MODE_REL);

	return 0;
}

static void __exit hrtimer_demo_exit_module(void)
{
	/* Step4. cancel the high timer
	 * int hrtimer_cancel(struct hrtimer *timer)
	 */
	hrtimer_cancel(&timer);
	printk("[hrtimer demo] cancel hrtimer\n");
}

module_init(hrtimer_demo_init_module);
module_exit(hrtimer_demo_exit_module);
MODULE_AUTHOR("HeZhiping");
MODULE_LICENSE("Dual BSD/GPL");
MODULE_DESCRIPTION("A simple demo to show how to use hrtimer");
