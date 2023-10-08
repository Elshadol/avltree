#ifndef	AVLTREE_INCLUDED
#define	AVLTREE_INCLUDED

#include <stddef.h>

struct avl_node {
    unsigned long __avl_parent_bf;
#define AVL_BALANCED    0lu        // 00
#define AVL_RIGHT_HEAVY 1lu        // 01
#define AVL_LEFT_HEAVY  2lu        // 10
    struct avl_node *avl_right;
    struct avl_node *avl_left;
} __attribute__((aligned(sizeof(long))));

struct avl_root {
    struct avl_node *avl_node;
};

#define avl_parent(n) \
    ((struct avl_node *)((n)->__avl_parent_bf & ~3lu))
#define avl_bf(n) \
    ((unsigned long)((n)->__avl_parent_bf & 3lu))

static inline void avl_set_parent(struct avl_node *n,
                                  const struct avl_node *p)
{
    n->__avl_parent_bf =
        (n->__avl_parent_bf & 3lu) | (unsigned long)(p);
}

static inline void avl_set_bf(struct avl_node *n,
                              unsigned long b)
{
    n->__avl_parent_bf =
        (n->__avl_parent_bf & ~3lu) | (unsigned long)(b);
}

#define avl_left_heavy(n) (avl_bf(n) == AVL_LEFT_HEAVY)
#define avl_right_heavy(n) (avl_bf(n) == AVL_RIGHT_HEAVY)
#define avl_balanced(n) (avl_bf(n) == AVL_BALANCED)

#define AVL_ROOT (struct avl_root) { NULL, }

#define	avl_entry(ptr, type, member) __extension__ ({ \
	const __typeof__(((type *)0)->member) *__pmember = (ptr); \
	(type *)((char *)__pmember - offsetof(type, member)); })

#define avl_entry_safe(ptr, type, member) __extension__ ({ \
        const typeof(ptr) ___ptr = (ptr); \
        ___ptr ? avl_entry(___ptr, type, member) : NULL; })

#define AVL_EMPTY_ROOT(root) ((root)->avl_node == NULL)
#define AVL_EMPTY_NODE(node) ((avl_parent((node)) == (node))
#define AVL_CLEAR_NODE(node) (avl_set_parent((node), (node)))

void avl_insert_fixup(struct avl_node *inserted, struct avl_root *root);
void avl_erase(struct avl_node *delete, struct avl_root *root);

struct avl_node *avl_next(const struct avl_node *);
struct avl_node *avl_prev(const struct avl_node *);
struct avl_node *avl_first(const struct avl_root *);
struct avl_node *avl_last(const struct avl_root *);

void avl_replace_node(struct avl_node *victim, struct avl_node *newnode,
                      struct avl_root *root);

static inline void avl_link_node(struct avl_node *node,
                                 const struct avl_node *parent,
                                 struct avl_node **avl_link)
{
    node->__avl_parent_bf = (unsigned long)parent;
    node->avl_left = NULL;
    node->avl_right = NULL;
    *avl_link = node;
}

static inline void avl_erase_init(struct avl_node *node, struct avl_root *root)
{
    avl_erase(node, root);
    AVL_CLEAR_NODE(node);
}

#endif	/* avlTREE_INCLUDED */
