/*
 * a fast, compact and non-recursive avl tree implementation.
 * with the same API to linux rbtree
 */

#ifndef	AVLTREE_INCLUDED
#define	AVLTREE_INCLUDED

#include <stddef.h>

struct avl_node {
    struct avl_node *avl_parent;
    struct avl_node *avl_left;
    struct avl_node *avl_right;
    int avl_bf;
#define AVL_LEFT_HEAVY -1
#define AVL_BALANCED 0
#define AVL_RIGHT_HEAVY 1
};

struct avl_root {
    struct avl_node *avl_node;
};

#define AVL_ROOT (struct avl_root) { NULL, }
#define	avl_entry(ptr, type, member) \
	(type*)( ((char*)((type*)ptr)) - ((size_t) &((type *)0)->member) )
	
void avl_insert_fixup(struct avl_node *, struct avl_root *);
void avl_erase(struct avl_node *, struct avl_root *);

struct avl_node *avl_next(const struct avl_node *);
struct avl_node *avl_prev(const struct avl_node *);
struct avl_node *avl_first(const struct avl_root *);
struct avl_node *avl_last(const struct avl_root *);

void avl_replace_node(struct avl_node *victim, struct avl_node *newnode,
                             struct avl_root *root);

static inline void avl_link_node(struct avl_node *node, struct avl_node *parent,
                                 struct avl_node **avl_link)
{
    node->avl_parent = parent;
    node->avl_left = node->avl_right = NULL;
    node->avl_bf = AVL_BALANCED;
    *avl_link = node;
}

#endif	/* AVLTREE_INCLUDED */
