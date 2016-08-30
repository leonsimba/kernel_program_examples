/*
 * htables_demo.c -- simple demonstration of hash table
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

#include <linux/hash.h> 	/* for hash table*/

#define CHANNEL_HTSIZE  (512)
#define CHANNEL_HTMASK	(CHANNEL_HTSIZE-1)

struct {

};

struct Qdisc_class_common
{               
	u32                     classid;
	struct hlist_node       hnode;
};

struct Qdisc_class_hash {
	struct hlist_head 	*hash;
	unsigned int            hashsize;
	unsigned int            hashmask;
	unsigned int            hashelems;
};

static inline void sch_tree_lock(const struct Qdisc *q)
{
	spin_lock_bh(qdisc_root_sleeping_lock(q));
}

static struct hlist_head *hash_table_alloc(unsigned int n)
{
	unsigned int size = n * sizeof(struct hlist_head), i;
	struct hlist_head *h;

	if (size <= PAGE_SIZE)
		h = kmalloc(size, GFP_KERNEL);
	else
		h = (struct hlist_head *)
			__get_free_pages(GFP_KERNEL, get_order(size));

	if (h != NULL) {
		for (i = 0; i < n; i++)
			INIT_HLIST_HEAD(&h[i]);
	}
	return h;
}

static void hash_table_free(struct hlist_head *h, unsigned int n)
{
	unsigned int size = n * sizeof(struct hlist_head);

	if (size <= PAGE_SIZE)
		kfree(h);
	else
		free_pages((unsigned long)h, get_order(size));
}

void hash_table_grow(struct Qdisc *sch, struct Qdisc_class_hash *clhash)
{
	struct Qdisc_class_common *cl;
	struct hlist_node *next;
	struct hlist_head *nhash, *ohash;
	unsigned int nsize, nmask, osize;
	unsigned int i, h;

	/* Rehash when load factor exceeds 0.75 */
	if (clhash->hashelems * 4 <= clhash->hashsize * 3)
		return;
	nsize = clhash->hashsize * 2;
	nmask = nsize - 1;
	nhash = hash_table_alloc(nsize);
	if (nhash == NULL)
		return;

	ohash = clhash->hash;
	osize = clhash->hashsize;

	sch_tree_lock(sch);
	for (i = 0; i < osize; i++) {
		hlist_for_each_entry_safe(cl, next, &ohash[i], hnode) {
			h = qdisc_class_hash(cl->classid, nmask);
			hlist_add_head(&cl->hnode, &nhash[h]);
		}
	}
	clhash->hash     = nhash;
	clhash->hashsize = nsize;
	clhash->hashmask = nmask;
	sch_tree_unlock(sch);

	hash_table_free(ohash, osize);
}
EXPORT_SYMBOL(hash_table_grow);

int hash_table_init(struct Qdisc_class_hash *clhash)
{
	unsigned int size = 4;

	clhash->hash = hash_table_alloc(size);
	if (clhash->hash == NULL)
		return -ENOMEM;
	clhash->hashsize  = size;
	clhash->hashmask  = size - 1;
	clhash->hashelems = 0;
	return 0;
}
EXPORT_SYMBOL(hash_table_init);

void qdisc_class_hash_destroy(struct Qdisc_class_hash *clhash)
{
	hash_table_free(clhash->hash, clhash->hashsize);
}
EXPORT_SYMBOL(qdisc_class_hash_destroy);

void qdisc_class_hash_insert(struct Qdisc_class_hash *clhash,
			     struct Qdisc_class_common *cl)
{
	unsigned int h;

	INIT_HLIST_NODE(&cl->hnode);
	h = qdisc_class_hash(cl->classid, clhash->hashmask);
	hlist_add_head(&cl->hnode, &clhash->hash[h]);
	clhash->hashelems++;
}
EXPORT_SYMBOL(qdisc_class_hash_insert);

void qdisc_class_hash_remove(struct Qdisc_class_hash *clhash,
			     struct Qdisc_class_common *cl)
{
	hlist_del(&cl->hnode);
	clhash->hashelems--;
}
EXPORT_SYMBOL(qdisc_class_hash_remove);
/************************************************************/
static struct hlist_head channel_htable[CHANNEL_HTSIZE];

static __u32 channel_name_key(char *name, size_t name_len)
{
	int i;
	__u32 key, *v;
	
	key = 0;
	v = (__u32 *)name;
	key = GOLDEN_RATIO_PRIME_32;

	for (i=0; i < name_len/sizeof(__u32); i++)
		key *= v[i];

	if (name_len % sizeof(__u32)) {
		for (i *= sizeof(__u32); i<name_len; i++)
			key *= name[i];
	}

	return key & CHANNEL_HTMASK;
}

static struct avmi_channel *avmi_channel_find_by_name(char *name, size_t name_len)
{
	__u32 key;
	struct hlist_head *head;
	struct hlist_node *hnode;
	struct avmi_channel *ch;

	ch = NULL;
	key = channel_name_key(name, name_len);
	head = &channel_htable[key];
	spin_lock_bh(&channel_pool_lock);
	hlist_for_each(hnode, head) {
		ch = hlist_entry(hnode, struct avmi_channel, hnode);
		if (!strncmp(ch->name, name, AVMI_CHANNEL_NAME_LEN)) {
			avmi_channel_get(ch);
			break;
		}
	}
	spin_unlock_bh(&channel_pool_lock);

	if (hnode)
		return ch;
	return NULL;
}

static long channel_insert_by_name(struct avmi_channel *new_ch)
{
	__u32 key;
	struct hlist_head *head = NULL;
	struct hlist_node *hnode = NULL;
	struct avmi_channel *ch;

	key = channel_name_key(new_ch->name, strlen(new_ch->name));
	head = &channel_htable[key];

	spin_lock_bh(&channel_pool_lock);
	hlist_for_each(hnode, head) {
		ch = hlist_entry(hnode, struct avmi_channel, hnode);
		if (!strncmp(ch->name, new_ch->name, AVMI_CHANNEL_NAME_LEN)) {
			break;
		}
	}
	if (!hnode) {
		hlist_add_head(&new_ch->hnode, head);
		avmi_channel_get(new_ch);
	}
	spin_unlock_bh(&channel_pool_lock);

	return hnode ? -EEXIST : 0;
}

static void channel_delete_by_name(struct avmi_channel *ch)
{
	__u32 key;
	struct hlist_head *head;

	key = channel_name_key(ch->name, strlen(ch->name));
	head = &channel_htable[key];

	spin_lock_bh(&channel_pool_lock);
	if (!hlist_unhashed(&ch->hnode))
		hlist_del_init(&ch->hnode);
	spin_unlock_bh(&channel_pool_lock);

	avmi_channel_put(ch);
}

static void init_hash_table(void)
{
	long err;
	for (err=0; err<CHANNEL_HTSIZE ; err++)
		INIT_HLIST_HEAD(&channel_htable[err]);
}

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
MODULE_DESCRIPTION("Simple demonstration of the hash table.");
