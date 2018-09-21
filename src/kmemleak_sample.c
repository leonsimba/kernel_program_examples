/*
 * kmemleak_sample.c -- A memory leak sample
 *
 * OS 		: CentOS-7.2-1151
 * Kernel 	: Linux-3.10.0-327.el7.x86_64.debug
 *
 * Author 	: Zhiping He
 *
 * The source code in this file can be freely used, adapted,
 * and redistributed in source or binary form, so long as an
 * acknowledgment appears in derived source files.
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/mm.h>
#include <linux/slab.h>
#include <asm/page.h>

static void kmemleak_alloc_memory(void)
{	
	void *ptr;

	ptr = kmalloc(310, GFP_KERNEL);
	if (!ptr) {
		printk("[Kmemleak]: kmalloc failed\n");
		return;
	}

	printk("[Kmemleak]: memory address %p \n", ptr);
}

int __init kmemleak_sample_init(void)
{	
	printk("[Kmemleak]: kmemleak_sample_init.\n");
	kmemleak_alloc_memory();

	return 0;
}

void __exit kmemleak_sample_exit(void)
{
	printk("[Kmemleak]: kmemleak_sample_exit.\n");
}

module_init(kmemleak_sample_init);
module_exit(kmemleak_sample_exit);
MODULE_AUTHOR("Zhiping He");
MODULE_LICENSE("Dual BSD/GPL");
