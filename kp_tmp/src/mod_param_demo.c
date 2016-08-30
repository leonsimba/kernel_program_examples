/*
 * modparam_demo.c --  Simple demonstration of module parameters
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

static char *book_type = "history";
module_param(book_type, charp, S_IRUGO);
MODULE_PARM_DESC(book_type, "the type of the book");

static int book_num = 10;
module_param(book_num, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
MODULE_PARM_DESC(book_num, "the number of the book");

static uint inner = 1;
module_param_named(outter, inner, uint, S_IRUGO);

#define DESC_MAXLEN    128
static char book_desc[DESC_MAXLEN]= "none description";
module_param_string(book_description, book_desc, DESC_MAXLEN, S_IRUGO | S_IWUSR);

#define MAX_NUM        8
static int array[MAX_NUM];
static int nr_array;
module_param_array(array, int, &nr_array, 0444);

static int __init modparam_demo_init(void)
{
	int i;
	printk("book_type = %s\n", book_type);
	printk("book_num  = %d\n", book_num);
	printk("book_desc = %s\n", book_desc);
	printk("outter    = %u\n", inner);
	printk("nr_array  = %u\n", nr_array);

	if (nr_array > 0) {
		for (i = 0; i < nr_array; i++)
			printk("array[%d] = %d\n", i, array[i]);	
	}
	return 0;
}

static void __exit modparam_demo_exit(void)
{
	printk("exit mod param\n");
}

module_init(modparam_demo_init);
module_exit(modparam_demo_exit);
MODULE_AUTHOR("HeZhiping");
MODULE_LICENSE("Dual BSD/GPL");
MODULE_DESCRIPTION("Simple demonstration of module parameters");
