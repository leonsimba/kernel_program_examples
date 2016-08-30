
struct xnet_kernel_ticket {
	spinlock_t lock; 		/* protect rb_tree */
	struct rb_root user_tickets;
};

struct xnet_user_ticket {
	atomic_t refcnt;
	struct rb_node node;
	struct my_data data;
};

static inline struct xnet_user_ticket *user_tickets_lookup(__u32 port)
{
	struct rb_node *node = NULL;
	struct xnet_user_ticket *user_ticket = NULL;

	spin_lock_bh(&kernel_ticket.lock);
	node = kernel_ticket.user_tickets.rb_node;

	while (node) {
		user_ticket = container_of(node, struct xnet_user_ticket, node);
		if (port < user_ticket->port)
			node = node->rb_left;
		else if (port > user_ticket->port)
			node = node->rb_right;
		else
			break;
		user_ticket = NULL;
	}

	if (user_ticket)
		user_ticket_get(user_ticket);

	spin_unlock_bh(&kernel_ticket.lock);
	return user_ticket;
}

static inline int user_ticket_insert(struct xnet_user_ticket *user_ticket)
{
	struct rb_node **node = NULL;
	struct rb_node *parent = NULL;
	struct xnet_user_ticket *cur = NULL;
	int ret;

	spin_lock_bh(&kernel_ticket.lock);
	node = &kernel_ticket.user_tickets.rb_node;

	while (*node) {
		cur = container_of(*node, struct xnet_user_ticket, node);

		parent = *node;
		if (user_ticket->port < cur->port)
			node = &((*node)->rb_left);
		else if (user_ticket->port > cur->port)
			node = &((*node)->rb_right);
		else
			break;
		cur = NULL;
	}

	if (cur)
		ret = -EBUSY;
	else {
		rb_link_node(&user_ticket->node, parent, node);
		rb_insert_color(&user_ticket->node,
				&kernel_ticket.user_tickets);
		user_ticket_get(user_ticket);
		ret = 0;
	}

	spin_unlock_bh(&kernel_ticket.lock);
	return ret;
}

static inline void user_ticket_delete(struct xnet_user_ticket *ticket)
{
	spin_lock_bh(&kernel_ticket.lock);
	rb_erase(&ticket->node, &kernel_ticket.user_tickets);
	RB_CLEAR_NODE(&ticket->node);
	spin_unlock_bh(&kernel_ticket.lock);
	user_ticket_put(ticket);
}

static inline void user_ticket_destroy_all(void)
{
	struct rb_root *root = NULL;
	struct rb_node *node;

	spin_lock_bh(&kernel_ticket.lock);
	root = &kernel_ticket.user_tickets;

	while (NULL != (node = rb_first(root))) {
		struct xnet_user_ticket *ticket;

		ticket = container_of(node, struct xnet_user_ticket, node);

		rb_erase(&ticket->node, root);
		RB_CLEAR_NODE(&ticket->node);

		spin_unlock_bh(&kernel_ticket.lock);
		BUG_ON(1 != atomic_read(&ticket->refcnt));
		user_ticket_put(ticket);
		spin_lock_bh(&kernel_ticket.lock);
	}
	spin_unlock_bh(&kernel_ticket.lock);
}
