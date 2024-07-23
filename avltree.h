#ifndef AVL_H_
#define AVL_H_

#include <stddef.h>

struct avl_node {
  unsigned long __avl_parent_balance;
#define AVL_BALANCED 0lu    // 00
#define AVL_RIGHT_HEAVY 1lu // 01
#define AVL_LEFT_HEAVY 2lu  // 10
  struct avl_node *avl_right;
  struct avl_node *avl_left;
} __attribute__((aligned(sizeof(long))));

struct avl_root {
  struct avl_node *avl_node;
};

#define AVL_ROOT                                                               \
  (struct avl_root) { NULL, }

#define avl_entry(ptr, type, member)                                           \
  __extension__({                                                              \
    const __typeof__(((type *)0)->member) *__pmember = (ptr);                  \
    (type *)((char *)__pmember - offsetof(type, member));                      \
  })

#define avl_entry_safe(ptr, type, member)                                      \
  __extension__({                                                              \
    const typeof(ptr) ___ptr = (ptr);                                          \
    ___ptr ? avl_entry(___ptr, type, member) : NULL;                           \
  })

#define AVL_EMPTY_NODE(node)                                                   \
  ((node)->__avl_parent_balance == (unsigned long)(node))
#define AVL_CLEAR_NODE(node)                                                   \
  ((node)->__avl_parent_balance = (unsigned long)(node))

void avl_insert_rebalance(struct avl_node *, struct avl_root *);
void avl_erase(struct avl_node *delete, struct avl_root *);

struct avl_node *avl_next(const struct avl_node *);
struct avl_node *avl_prev(const struct avl_node *);
struct avl_node *avl_first(const struct avl_root *);
struct avl_node *avl_last(const struct avl_root *);

void avl_replace_node(struct avl_node *victim, struct avl_node *newnode,
                      struct avl_root *root);

static inline void avl_link_node(struct avl_node *node, struct avl_node *parent,
                                 struct avl_node **avl_link) {
  node->__avl_parent_balance = (unsigned long)parent;
  node->avl_left = NULL;
  node->avl_right = NULL;
  *avl_link = node;
}

static inline void avl_erase_init(struct avl_node *node,
                                  struct avl_root *root) {
  avl_erase(node, root);
  AVL_CLEAR_NODE(node);
}

#endif
