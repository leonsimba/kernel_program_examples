/*
 * kernel_thread_demo.c --  Simple demonstration of kernel thread.
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
#include <linux/sched.h>	/* for CLONE flag, daemonize() */
#include <asm/processor.h> 	/* for kernel_thread() */

/* define a wait queue */
static DECLARE_WAIT_QUEUE_HEAD(my_thread_wait);

/* define a completion */
static DECLARE_COMPLETION(my_thread_exit); 

/* Exit Flag */
int need_exit = 0; 

/* Helper thread */
static int my_thread(void *unused)
{
	/* define a wait queue entry */
	DECLARE_WAITQUEUE(wait, current);
	daemonize("my_thread");
	add_wait_queue(&my_thread_wait, &wait);
	
	/* Do not block SIGKILL signal*/
	allow_signal(SIGKILL);

	while (1) {
		/* Give up processor until event occurs */
		set_current_state(TASK_INTERRUPTIBLE);
		schedule();
		
		/* break the loop when need_exit become true 
		 * or receive SIGKILL */
		if (need_exit || signal_pending(current))
			break;
	
		/* Do the real work here */
	}
	
	__set_current_state(TASK_RUNNING);
	remove_wait_queue(&my_thread_wait, &wait);
	
	/* Atomically signal completion and exit */
	complete_and_exit(&my_thread_exit, 0);
}

static int __init kernel_thread_demo_init(void)
{
	int err;

	/* create a kernel thread */
	err = kernel_thread(my_thread, NULL,
			CLONE_FS | CLONE_FILES | CLONE_SIGHAND | SIGCHLD);
	if (err < 0) {
		printk("kernel_thread failed, %s\n", (char *)ERR_PTR(err));
		return err;
	}
	return 0;
}

static void __exit kernel_thread_demo_exit(void)
{
	/* my_thread need exit */
	need_exit = 1; 

	/* wake up my_thread */
	wake_up(&my_thread_wait);

	/* Wait until my_thread quits */
	wait_for_completion(&my_thread_exit); 
} 

module_init(kernel_thread_demo_init);
module_exit(kernel_thread_demo_exit);
MODULE_AUTHOR("HeZhiping");
MODULE_LICENSE("Dual BSD/GPL");
MODULE_DESCRIPTION("A simple demo of kernel_thread");
