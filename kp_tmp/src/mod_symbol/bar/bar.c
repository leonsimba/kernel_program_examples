#include <linux/init.h>
#include <linux/module.h>

void bar_hello_world(void)
{
	printk("bar: hello world\n");
}
EXPORT_SYMBOL(bar_hello_world);

static int __init bar_init(void)
{
	printk("insmod module bar\n");
	return 0;
}

static void __exit bar_exit(void)
{
	printk("rmmod module bar\n");
}

module_init(bar_init);
module_exit(bar_exit);
MODULE_AUTHOR("hzp");
MODULE_LICENSE("Dual BSD/GPL");
