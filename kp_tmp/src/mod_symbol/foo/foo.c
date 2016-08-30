#include <linux/module.h>
#include <linux/init.h>

extern void bar_hello_world(void);

static int __init foo_init(void)
{
	printk("Insmod module foo\n");
	bar_hello_world();
	return 0;
}

static void __exit foo_exit(void)
{
	printk("rmmod module foo\n");
}

module_init(foo_init);
module_exit(foo_exit);
MODULE_AUTHOR("hzp");
MODULE_LICENSE("Dual BSD/GPL");
