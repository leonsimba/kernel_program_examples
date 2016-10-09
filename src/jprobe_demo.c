/*
 * jprobe_demo.c --  An example to show how to use jprobe.
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

struct jprobe jp;

int jp_do_execve(char *filename, char __user *__user *argv,
	char __user *__user *envp, struct pt_regs *regs)
{
	int cnt = 0;

	printk(KERN_INFO "filename: %s\n", (char *)regs->di);
	for (; *argv != NULL; argv++, cnt++)
		printk(KERN_INFO "argv[%d]: %s\n", cnt, *argv);

	jprobe_return();
	return 0;
}

static int __init init_jprobe_samples(void)
{
	jp.kp.symbol_name = "do_execve";
	jp.entry = JPROBE_ENTRY(jp_do_execve);

	register_jprobe(&jp);
	return 0;
}

static void __exit exit_jprobe_samples(void)
{
	unregister_kprobe(&jp);
}

module_init(init_jprobe_samples);
module_exit(exit_jprobe_samples);
MODULE_AUTHOR("HeZhiping");
MODULE_LICENSE("Dual BSD/GPL");
MODULE_DESCRIPTION("A simple demo to show how to use jprobe");
