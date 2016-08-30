/*
 * curtime_demo.c -- Simple demonstration of get current time.
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

#include <linux/time.h>
#include <linux/proc_fs.h>
#include <linux/types.h>

/* This file, on the other hand, returns the current time forever */
int curtime_currentime(char *buf, char **start, off_t offset,
                   int len, int *eof, void *data)
{
	struct timeval tv1;
	struct timespec tv2;
	unsigned long j1;
	u64 j2;

	j1 = jiffies;
	j2 = get_jiffies_64();
	do_gettimeofday(&tv1);
	tv2 = current_kernel_time();

	len=0;
	len += sprintf(buf,"0x%08lx 0x%016Lx %10i.%06i\n"
		       "%40i.%09i\n",
		       j1, j2,
		       (int) tv1.tv_sec, (int) tv1.tv_usec,
		       (int) tv2.tv_sec, (int) tv2.tv_nsec);
	*start = buf;
	return len;
}

static int __init curtime_demo_init(void)
{
	create_proc_read_entry("currentime", 0, NULL, curtime_currentime, NULL);
	return 0;
}

static void __exit curtime_demo_cleanup(void)
{
	remove_proc_entry("currentime", NULL);
}

module_init(curtime_demo_init);
module_exit(curtime_demo_cleanup);
MODULE_AUTHOR("HeZhiping");
MODULE_LICENSE("Dual BSD/GPL");
MODULE_DESCRIPTION("Simple demonstration of get current time.");
