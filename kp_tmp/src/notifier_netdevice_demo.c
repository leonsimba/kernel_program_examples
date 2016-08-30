#include <asm/uaccess.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/notifier.h>     // for notifier API
#include <linux/netdevice.h>    // for netdevice notifier API

#define RETRY_NUM 0x5000

static int clean_loopback_refcnt(struct notifier_block *nb, unsigned long event, void *v)
{
        struct net_device *dev = v;
        int temp;
        atomic_t value;

        if (dev == NULL)
                return NOTIFY_DONE;

        atomic_set(&value, 0);
        switch(event){
        case NETDEV_UNREGISTER:
                if (strcmp(dev->name, "lo") == 0 &&
                        atomic_read(&dev->refcnt) != 0 )
                {
                        temp = dev->num_tx_queues;
                        atomic_set(&value, temp);
                        dev->num_tx_queues += 0x1000;

                        if (atomic_sub_return(RETRY_NUM, &value) > 0) {
                                printk("loopback dev->refcnt = 0x%0x\n", atomic_read(&dev->refcnt));
                                atomic_set(&dev->refcnt, 0);
                        }

                        /* restore the num_tx_queues */
                        if (atomic_read(&dev->refcnt) == 0)
                                dev->num_tx_queues = dev->num_tx_queues % 0x1000;
                }
                break;
        default :
                break;
        }

        return NOTIFY_DONE;
}

static struct notifier_block clean_loopback_notifier  = {
        .notifier_call = clean_loopback_refcnt,
        .priority = 0,
};

static int __init clean_lo_refcnt_init(void)
{
        int err;

        err = register_netdevice_notifier(&clean_loopback_notifier);
        if (err)
                return err;

        return 0;
}

static void __exit clean_lo_refcnt_exit(void)
{
        unregister_netdevice_notifier(&clean_loopback_notifier);
}

module_init(clean_lo_refcnt_init);
module_exit(clean_lo_refcnt_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("He ZhiPing <zhiping.hzp@alibaba-inc.com>");
MODULE_DESCRIPTION("A workaround method to fix loopback refcnt leak on T4 platform");
