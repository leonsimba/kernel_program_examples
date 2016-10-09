/*
 * kprobe_demo.c --  An example to show how to use kprobe.
 *
 * Copyright (C) 2016 He zhiping
 * Version : Linux-3.0.76
 *
 * The source code in this file can be freely used, adapted,
 * and redistributed in source or binary form, so long as an
 * acknowledgment appears in derived source files.
 *
 */

#include <linux/kernel.h>	/* printk() */
#include <linux/module.h>

#include <linux/kprobes.h>	/* for kprobe */
#include <linux/kallsyms.h>

struct kprobe kp;

/* case1: show global variable */
int handler_pre(struct kprobe *p, struct pt_regs *regs)
{
	printk(KERN_INFO "pt_regs: %p, pud: %d, jiffies: %ld\n",
		regs, current->tgid, jiffies);
	return 0;
}

#if 0
/* case2: show the input parameters of the function */
int handler_pre(struct kprobe *p, struct pt_regs *regs)
{
	int cnt = 0;
	char __user *__user *argv;

	printk(KERN_INFO "filename: %s\n", (char *)regs->di);
	for (argv = (char __user *__user *)regs->si; *argv != NULL; argv++, cnt++)
		printk(KERN_INFO "argv[%d]: %s\n", cnt, *argv);

	return 0;
}

/* case2: show the stack */
int handler_pre(struct kprobe *p, struct pt_regs *regs)
{
	dump_stack();
	return 0;
}
#endif

static int __init init_kprobe_samples(void)
{
	// cat /proc/kallsyms | grep do_execve
	kp.addr = (kprobe_opcode_t *) 0xffffffff81166f20;
	kp.pre_handler = handler_pre;

	register_kprobe(&kp);

	return 0;
}

static void __exit exit_kprobe_samples(void)
{
	unregister_kprobe(&kp);
}

module_init(init_kprobe_samples);
module_exit(exit_kprobe_samples);
MODULE_AUTHOR("HeZhiping");
MODULE_LICENSE("Dual BSD/GPL");
MODULE_DESCRIPTION("A simple demo to show how to use kprobe");
