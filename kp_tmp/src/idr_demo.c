/*
 * idr_demo.c -- An example to show how to use idr tree
 *
 * Copyright (C) 2015
 * Version : Linux-2.6.32
 * Author  : zhiping.he, zhiping.hzp@alibaba-inc.com
 *
 * The source code in this file can be freely used, adapted,
 * and redistributed in source or binary form, so long as an
 * acknowledgment appears in derived source files.
 */
#include <linux/kernel.h>
#include <linux/module.h>

#include <linux/errno.h>
#include <linux/fs.h>
#include <linux/sched.h>
#include <linux/parser.h>
#include <linux/slab.h>

#include <linux/idr.h>

/**
 * struct idr_idpool - per-connection accounting for tag idpool
 * @lock: protects the pool
 * @pool: idr to allocate tag id from
 */
struct idr_idpool {
	spinlock_t lock;
	struct idr pool;
};

/**
 * idr_idpool_create - create a new per-connection id pool
 */
struct idr_idpool *idr_idpool_create(void)
{
	struct idr_idpool *p;

	p = kmalloc(sizeof(struct idr_idpool), GFP_KERNEL);
	if (!p)
		return ERR_PTR(-ENOMEM);

	spin_lock_init(&p->lock);
	idr_init(&p->pool);

	return p;
}
EXPORT_SYMBOL(idr_idpool_create);

/**
 * idr_idpool_destroy - create a new per-connection id pool
 * @p: idpool to destroy
 */
void idr_idpool_destroy(struct idr_idpool *p)
{
	/* do some clean operations (optional)*/
	//idr_for_each(&p->pool, idr_callback, data);

	/* free all idr_layer objects in idr tree */
	idr_remove_all(&p->pool);

	/* free all idr_layer objects which cached in id_free list */
	idr_destroy(&p->pool);

	kfree(p);
}
EXPORT_SYMBOL(idr_idpool_destroy);

/**
 * idr_idpool_get - allocate numeric id from pool
 * @p: pool to allocate from
 *
 * Bugs: This seems to be an awful generic function, should it be in idr.c with
 *            the lock included in struct idr?
 */

int idr_idpool_get(struct idr_idpool *p, void *ptr)
{
	int id;
	int ret;
	unsigned long flags;

	if (!ptr)
		return -EINVAL;
retry:
	ret = -ENOMEM;
	if (unlikely(!idr_pre_get(&p->pool, GFP_KERNEL)))
		 goto out_err;

	spin_lock_irqsave(&p->lock, flags);
	ret = idr_get_new(&p->pool, ptr, &id);
	spin_unlock_irqrestore(&p->lock, flags);
	if (ret < 0) {
		/* idr was out of memory allocate and try again */
		if (ret == -EAGAIN)
			goto retry;
		goto out_err;
	}

	ret = id;
	printk("pool %p, id = %d\n", p, id);
out_err:
	return ret;
}
EXPORT_SYMBOL(idr_idpool_get);

/**
 * idr_idpool_put - release numeric id from pool
 * @id: numeric id which is being released
 * @p: pool to release id into
 *
 * Bugs: This seems to be an awful generic function, should it be in idr.c with
 *            the lock included in struct idr?
 */
void idr_idpool_put(int id, struct idr_idpool *p)
{
	unsigned long flags;

	spin_lock_irqsave(&p->lock, flags);
	idr_remove(&p->pool, id);
	spin_unlock_irqrestore(&p->lock, flags);
}
EXPORT_SYMBOL(idr_idpool_put);

/**
 * idr_idpool_check - check if the specified id is available
 * @id: id to check
 * @p: pool to check
 */
int idr_idpool_check(int id, struct idr_idpool *p)
{
	return idr_find(&p->pool, id) != NULL;
}
EXPORT_SYMBOL(idr_idpool_check);
