/*
 * htable_fix_length.c -- Simple demonstration of a fix length hash table
 *
 * This module is a silly one: it only embeds short code fragments
 * that show how time delays can be handled in the kernel.
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

#include <linux/mm.h> 		/* for memory alloc APIs */
#include <linux/list.h> 	/* for hash APIs */
#include <linux/hash.h> 	/* for GOLDEN_RATIO_PRIME_32 */
#include <linux/types.h>
#include <linux/spinlock.h>

#define NAME_LENGTH 		32
#define HTABLE_SIZE		32
#define HTABLE_MASK  		(HTABLE_SIZE - 1)
static spinlock_t my_htable_lock; 	/* used to protect my_htable */
static unsigned int htable_size = 0;
static struct hlist_head my_htable[HTABLE_SIZE];
static struct my_item *item1 = NULL;
static struct my_item *item2 = NULL;

struct my_item {
	char name[NAME_LENGTH];
	struct hlist_node  hnode; 	/* this is linked into the names hashtable */
	atomic_t count; 		/* reference count */
};


static inline void my_item_get(struct my_item *ch)
{
	atomic_inc(&ch->count);
}

static inline void my_item_put(struct my_item *ch)
{
	if (atomic_dec_and_test(&ch->count)) {
		kfree(ch);
		htable_size--;
		printk("htable_size = %d\n", htable_size);
	}
}

static struct my_item *my_item_alloc(char *name, int length)
{
	int len;
	struct my_item *data;

	data =  kzalloc(sizeof(struct my_item), GFP_KERNEL);
	if (!data)
		return ERR_PTR(-ENOMEM);

	len = length > NAME_LENGTH ? NAME_LENGTH : len;
	strncpy(data->name, name, len);
	atomic_set(&data->count, 1);
	INIT_HLIST_NODE(&data->hnode); 	/* init hlist_node */

	htable_size++;
	return data;
}

static __u32 hash_name(char *name, size_t name_len)
{
        int i;
        __u32 key, *v;

        v = (__u32 *)name;
        key = GOLDEN_RATIO_PRIME_32;
        for (i = 0; i < name_len/sizeof(__u32); i++)
                key *= v[i];
        if (name_len % sizeof(__u32)) {
                for (i *= sizeof(__u32); i<name_len; i++)
                        key *= name[i];
        }

        return key & HTABLE_MASK;
}

static long my_htable_insert_by_name(struct my_item *new_ch)
{
        __u32 key;
        struct hlist_head *head = NULL;
        struct hlist_node *hnode = NULL;
        struct my_item *ch;

        key = hash_name(new_ch->name, strlen(new_ch->name));
        head = &my_htable[key];

        spin_lock_bh(&my_htable_lock);
        hlist_for_each(hnode, head) {
                ch = hlist_entry(hnode, struct my_item, hnode);
                if (!strncmp(ch->name, new_ch->name, NAME_LENGTH)) {
                        break;
                }
        }
        if (!hnode) {
                hlist_add_head(&new_ch->hnode, head);
                my_item_get(new_ch);
        }
        spin_unlock_bh(&my_htable_lock);

        return hnode ? -EEXIST : 0;
}

static void my_htable_delete_by_name(struct my_item *ch)
{
        __u32 key;
        struct hlist_head *head;

        key = hash_name(ch->name, strlen(ch->name));
        head = &my_htable[key];

        spin_lock_bh(&my_htable_lock);
        if (!hlist_unhashed(&ch->hnode))
                hlist_del_init(&ch->hnode);
        spin_unlock_bh(&my_htable_lock);

        my_item_put(ch);
}

static struct my_item *my_htable_find_by_name(char *name, size_t name_len)
{
        __u32 key;
        struct hlist_head *head;
        struct hlist_node *hnode;
	struct my_item *ch = NULL;

        key = hash_name(name, name_len);
        head = &my_htable[key];

        spin_lock_bh(&my_htable_lock);
        hlist_for_each(hnode, head) {
        	ch = hlist_entry(hnode, struct my_item, hnode);
                if (!strncmp(ch->name, name, NAME_LENGTH)) {
                        my_item_get(ch);	/* increase refernece count */
                        break;
                }
        }
        spin_unlock_bh(&my_htable_lock);

        if (hnode)
                return ch;
        return NULL;
}

static int htable_simple_test(void)
{
	struct my_item *item;

	item1 = my_item_alloc("hello", 5);
	if (!item1)
		return -ENOMEM;
	my_htable_insert_by_name(item1);

	item2 = my_item_alloc("world", 5);
	if (!item2)
		return -ENOMEM;
	my_htable_insert_by_name(item2);

	item = my_htable_find_by_name("hello", 5);
	if (!item) {
		printk("Don't  find a item named = %s\n", item->name);
		return -EEXIST;
	}
	my_item_put(item); /* decrease reference count */

	my_htable_delete_by_name(item);

	item = my_htable_find_by_name("hello", 5);
	if (!item) {
		printk("Don't find a item named = hello\n");
		return -EEXIST;
	}
	my_item_put(item); /* decrease reference count */

	return 0;
}

static int __init htable_fix_length_demo_init(void)
{
	int i;

	for (i = 0; i < HTABLE_SIZE; i++)
		INIT_HLIST_HEAD(&my_htable[i]);
	spin_lock_init(&my_htable_lock);

	/* for test */
	htable_simple_test();
	return 0;
}

static void __exit htable_fix_length_demo_exit(void)
{
	int i;
	struct my_item *item = NULL;
	struct hlist_node *cur, *next;

        spin_lock_bh(&my_htable_lock);
	for (i = 0; i < HTABLE_SIZE; i++) {
		hlist_for_each_entry_safe(item, cur, next, &my_htable[i], hnode) {
			printk("name = %s, count = %d\n", item->name, atomic_read(&item->count));
		}
	}
        spin_unlock_bh(&my_htable_lock);

	if (item1) {
		my_htable_delete_by_name(item1);
		my_item_put(item);
	}

	if (item2) {
		my_htable_delete_by_name(item2);
		my_item_put(item);
	}

	if (htable_size)
		printk("leak %d my_item\n", htable_size);
	return;
}

module_init(htable_fix_length_demo_init);
module_exit(htable_fix_length_demo_exit);
MODULE_AUTHOR("HeZhiping");
MODULE_LICENSE("Dual BSD/GPL");
MODULE_DESCRIPTION("Simple demonstration of a fix length hash table.");
