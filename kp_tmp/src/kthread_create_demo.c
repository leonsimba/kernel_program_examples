/*
 * kthread_create_demo.c --  Simple demonstration of kthread_create.
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
#include <linux/wait.h>
#include <linux/sched.h>
#include <linux/kthread.h> 	/* for kthread_create() */

/* define a wait queue */
static DECLARE_WAIT_QUEUE_HEAD(my_thread_wait);

struct task_struct *my_task;

static int my_thread(void *unused)
{
	/* define a wait queue entry */
	DECLARE_WAITQUEUE(wait, current);
	add_wait_queue(&my_thread_wait, &wait);
	
	/* Continue work if no other thread has invoked kthread_stop() */
	while (!kthread_should_stop()) {
		/* Give up processor until kthread_stop() called */
		set_current_state(TASK_INTERRUPTIBLE);
		schedule();
		
		/* Do the real work here */
	}
	
	__set_current_state(TASK_RUNNING);
	remove_wait_queue(&my_thread_wait, &wait);
	return 0;
}

static int __init kthread_create_demo_init(void)
{
	int err = 0;

	/* create a kernel thread */
	my_task = kthread_create(my_thread, NULL, "%s", "my_thread");
	if (IS_ERR(my_task)) {
		printk(KERN_ERR "Create my_thread failed\n");
		err = PTR_ERR(my_task);
		my_task = NULL;	
		return err;
	}

	/* wake up my_thread */
	wake_up_process(my_task);

	return err;
}

static void __exit kthread_create_demo_exit(void)
{
	/* stop my_task */
	kthread_stop(my_task);
} 

module_init(kthread_create_demo_init);
module_exit(kthread_create_demo_exit);
MODULE_AUTHOR("HeZhiping");
MODULE_LICENSE("Dual BSD/GPL");
MODULE_DESCRIPTION("A simple demo of kthread_create.");
