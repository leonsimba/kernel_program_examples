/*
 * sysctl_demo.c -- simple demonstration of the sysctl
 *                  create /proc/sys/my_sysctl/{val1, val2, val3, mystring}
 *
 * Copyright (C) 2015 hezhiping
 * Version : Linux-2.6.32
 *
 * The source code in this file can be freely used, adapted,
 * and redistributed in source or binary form, so long as an
 * acknowledgment appears in derived source files.
 */
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/sysctl.h>

#define MY_MAX_SIZE 256
enum {
	MY_SYSCTL_VAL1 = 1,
	MY_SYSCTL_VAL2 = 2,
	MY_SYSCTL_VAL3 = 3,
	MY_SYSCTL_STRING = 4,
};

static int val1 = 0;	 /*0:disable , 1 :enable */
static int val2 = 1;	
static int min = 1;
static int max = 10;
static unsigned long val3 = 0; 	
static unsigned long lmin = 128;
static unsigned long lmax = 100000;
static char mystring[MY_MAX_SIZE];
static struct ctl_table_header *sysctl_demo_header;

static struct ctl_table my_sysctl_table[] = {
	{
		.ctl_name 	= MY_SYSCTL_VAL1,
		.procname 	= "val1",
		.data     	= &val1,
		.maxlen   	= sizeof(int),
 		.mode     	= 0666,
		.proc_handler 	= &proc_dointvec, 
	},
	{
		.ctl_name 	= MY_SYSCTL_VAL2,
		.procname 	= "val2",
		.data     	= &val2,
		.maxlen   	= sizeof(int),
 		.mode     	= 0666,
		.proc_handler 	= &proc_dointvec_minmax,
		.extra1	  	= &min,
		.extra2	  	= &max,
	},
	{
		.ctl_name 	= MY_SYSCTL_VAL3, 
		.procname 	= "val3",
		.data     	= &val3,
		.maxlen   	= sizeof(unsigned long),
 		.mode     	= 0666,
		.proc_handler 	= &proc_doulongvec_minmax,  
		.extra1  	= &lmin, 
		.extra2 	= &lmax,
	},
	{
		.ctl_name       = MY_SYSCTL_STRING,
		.procname       = "mystring",
		.data           = mystring,
		.maxlen         = MY_MAX_SIZE,
		.mode           = 0666,
		.proc_handler   = &proc_dostring,
		.strategy       = &sysctl_string,    // obsolete in new kernel version
	},
	{
		.ctl_name       = 0,
	}
};

#ifndef UNDER_DEV
#define MY_SYSCTL_ROOT	(CTL_CPU + 10)
static struct ctl_table sysctl_demo_root_table = {
	.ctl_name	= MY_SYSCTL_ROOT,
	.procname	= "my_sysctl",
	.mode		= 0555,
	.child		= my_sysctl_table, 
};

#else
#define MY_SYSCTL_ROOT  (DEV_IPMI + 10)
static struct ctl_table mid_sysctl_table[] = {
	{
		.ctl_name	= MY_SYSCTL_ROOT,
		.procname	= "my_sysctl",
		.mode		= 0555,
		.child		= my_sysctl_table, 
	},
	{}
};

static struct ctl_table sysctl_demo_root_table = {
	.ctl_name	= CTL_DEV,
	.procname	= "dev",
	.mode		= 0555,
	.child		= mid_sysctl_table, 
};
#endif

static int __init sysctl_demo_init(void)
{
	sysctl_demo_header = register_sysctl_table(&sysctl_demo_root_table);
	return 0;
}

static void __exit sysctl_demo_exit(void)
{
	unregister_sysctl_table(sysctl_demo_header);
}

module_init(sysctl_demo_init);
module_exit(sysctl_demo_exit);
MODULE_AUTHOR("HeZhiping");
MODULE_LICENSE("Dual BSD/GPL");
MODULE_DESCRIPTION("Simple demonstration of the sysctl.");
