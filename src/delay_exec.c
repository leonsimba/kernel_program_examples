/*
 * delay_exec.c -- Some methods of delay execution
 * Use:
 *	#dd bs=100 < /proc/busywait
 *
 * This module is a silly one: it only embeds short code fragments
 * that show how time delays can be handled in the kernel.
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

#include <linux/time.h>
#include <linux/sched.h>
#include <linux/proc_fs.h>
#include <linux/types.h>
#include <linux/spinlock.h>
#include <linux/interrupt.h>

#include <asm/hardirq.h>

/* the default delay, expressed in jiffies */
int delay = HZ;
module_param(delay, int, 0);

/* use these as data pointers, to implement four files in one function */
enum delay_method {
	DELAY_BUSY,
	DELAY_SCHED,
	DELAY_WAITQUEUE,
	DELAY_SCHEDTIMEOUT
};

/*
 * This function prints one line of data, after sleeping one second.
 * It can sleep in different ways, according to the data pointer
 */
int delay_method_funcs(char *buf, char **start, off_t offset,
	      int len, int *eof, void *data)
{
	int len1, len2;
	unsigned long j0, j1; /* jiffies */
	wait_queue_head_t wait;

	init_waitqueue_head (&wait);
	j0 = get_jiffies_64();
	j1 = j0 + delay;

	switch((long)data) {
		case DELAY_BUSY:
			while (time_before(jiffies, j1))
				cpu_relax();
			break;
		case DELAY_SCHED:
			while (time_before(jiffies, j1)) {
				schedule();
			}
			break;
		case DELAY_WAITQUEUE:
			wait_event_interruptible_timeout(wait, 0, delay);
			break;
		case DELAY_SCHEDTIMEOUT:
			set_current_state(TASK_INTERRUPTIBLE);
			schedule_timeout (delay);
			break;
	}
	j1 = get_jiffies_64(); /* actual value after we delayed */

	len1 = sprintf(buf, "before delay: %9li\n", j0);
	len2 = sprintf(buf+len1, "after  delay: %9li\n", j1);
	*start = buf;
	return (len1 + len2);
}

static int __init delay_exec_demo_init(void)
{
	create_proc_read_entry("busywait", 0, NULL, delay_method_funcs, (void *)DELAY_BUSY);
	create_proc_read_entry("sched",0, NULL, delay_method_funcs, (void *)DELAY_SCHED);
	create_proc_read_entry("waitqueue",0, NULL, delay_method_funcs, (void *)DELAY_WAITQUEUE);
	create_proc_read_entry("schedtimeout", 0, NULL, delay_method_funcs, (void *)DELAY_SCHEDTIMEOUT);

	return 0;
}

static void __exit delay_exec_demo_exit(void)
{
	remove_proc_entry("busywait", NULL);
	remove_proc_entry("sched", NULL);
	remove_proc_entry("waitqueue", NULL);
	remove_proc_entry("schedtimeout", NULL);
}

module_init(delay_exec_demo_init);
module_exit(delay_exec_demo_exit);
MODULE_AUTHOR("HeZhiping");
MODULE_LICENSE("Dual BSD/GPL");
MODULE_DESCRIPTION("Some methods of delay execution.");
