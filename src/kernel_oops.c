/*
 * kernel_oops.c --  An example to show kernel OOPs
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

static __init int init_oopsdemo(void)
{
	*((int *)0x00) = 0x19860307;
	return 0;
}
static __exit void exit_oopsdemo(void)
{
	return;
}

module_init(init_oopsdemo);
module_exit(exit_oopsdemo);
MODULE_AUTHOR("HeZhiping");
MODULE_LICENSE("Dual BSD/GPL");
MODULE_DESCRIPTION("A simple demo to show kernel OOPs");
