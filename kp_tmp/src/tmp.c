
/* define a waitquque */
static DECLARE_WAIT_QUEUE_HEAD(myevent_waitqueue);

rwlock_t myevent_lock;


extern unsigned int myevent_id;

static int mykthread(void *unused)
{
	unsigned int event_id = 0;

	/* define a waitquque entry */
	DECLARE_WAITQUEUE(wait, current);

	/* Become a kernel thread without attached user resources */
	daemonize("mykthread");
	
	/* Request delivery of SIGKILL */
	allow_signal(SIGKILL);
	
	/* The thread sleeps on this wait queue until it's
	*  woken up by parts of the kernel in charge of sensing
	*  the health of data structures of interest */
	add_wait_queue(&myevent_waitqueue, &wait);
	
	for (;;) {
		/* Relinquish the processor until the event occurs */
		set_current_state(TASK_INTERRUPTIBLE);

		/* Give up cpu */
		schedule();

		/* Die if I receive SIGKILL */
		if (signal_pending(current))
			break;
	
		/* Control gets here when the thread is woken up */
		read_lock(&myevent_lock); /* Critical section starts */
		if (myevent_id) { /* Guard against spurious wakeups */
			event_id = myevent_id;
			read_unlock(&myevent_lock); /* Critical section ends */

			/* Invoke the registered user mode helper and
			*  pass the identity code in its environment */
			run_umode_handler(event_id); /* Expanded later on */
		} else {
			read_unlock(&myevent_lock);
		}
	}
	
	set_current_state(TASK_RUNNING);
	remove_wait_queue(&myevent_waitqueue, &wait);
	return 0;
}

static int __init kernel_thread_demo_init(void)
{
	/* Initialize the lock to protect against concurrent list access */
	spin_lock_init(&mydrv_wq.lock);

	/* Initialize the wait queue for communication between the submitter and the worker */
	init_waitqueue_head(&mydrv_wq.todo);

	/* Initialize the list head */
	INIT_LIST_HEAD(&mydrv_wq.mydrv_worklist);

	/* Start the worker thread. See Listing 3.4 */
	kernel_thread(mydrv_worker, NULL, CLONE_FS | CLONE_FILES | CLONE_SIGHAND | SIGCHLD);

			    return 0;
}

module_init(kernel_thread_demo_init);
module_exit(proc_demo1_exit);
MODULE_AUTHOR("HeZhiping");
MODULE_LICENSE("Dual BSD/GPL");
MODULE_DESCRIPTION("A simple demo to show how to use procfs APIs");
