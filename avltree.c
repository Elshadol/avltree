#include "avltree.h"

static inline struct avl_node *avl_parent(const struct avl_node *n)
{
    return ((struct avl_node *)((n)->__avl_parent_bf & ~3lu));
}

static inline unsigned long avl_bf(const struct avl_node *n)
{
    return ((unsigned long)((n)->__avl_parent_bf & 3lu));
}

static inline void avl_set_parent(struct avl_node *n, const struct avl_node *p)
{
    n->__avl_parent_bf =
        (n->__avl_parent_bf & 3lu) | (unsigned long)(p);
}

static inline void avl_set_bf(struct avl_node *n, unsigned long b)
{
    n->__avl_parent_bf =
        (n->__avl_parent_bf & ~3lu) | (unsigned long)(b);
}

static inline void
__avl_change_child(struct avl_node *x, struct avl_node *y,
                  struct avl_node *x_parent, struct avl_root *root)
{
    if (x_parent) {
        if (x_parent->avl_left == x)
            x_parent->avl_left = y;
        else
            x_parent->avl_right = y;
    } else
        root->avl_node = y;
}

static inline void __avl_rotate_set_parents(struct avl_node *x,
        struct avl_node *y, struct avl_root *root)
{
    struct avl_node *x_parent = avl_parent(x);
    avl_set_parent(x, y);
    avl_set_parent(y, x_parent);
    __avl_change_child(x, y, x_parent, root);
}

void avl_insert_fixup(struct avl_node *x, struct avl_root *root)
{
    struct avl_node *x_parent, *y, *tmp;
    unsigned long bf;
    while ((x_parent = avl_parent(x))) {
        bf = avl_bf(x_parent);
        if (bf == AVL_RIGHT_HEAVY) {
            avl_set_bf(x_parent, AVL_BALANCED);
            if (x == x_parent->avl_right) {
                y = x->avl_left;
                bf = avl_bf(x);
                avl_set_bf(x, AVL_BALANCED);
                if (bf == AVL_LEFT_HEAVY) {
                    bf = avl_bf(y);
                    avl_set_bf(y, AVL_BALANCED);
                    if (bf == AVL_LEFT_HEAVY)
                        avl_set_bf(x, AVL_RIGHT_HEAVY);
                    else if (bf == AVL_RIGHT_HEAVY)
                        avl_set_bf(x_parent, AVL_LEFT_HEAVY);
                    x->avl_left = tmp = y->avl_right;
                    y->avl_right = x;
                    if (tmp)
                        avl_set_parent(tmp, x);
                    avl_set_parent(x, y);
                    x = y;
                    y = x->avl_left;
                }
                x_parent->avl_right = y;
                x->avl_left = x_parent;
                if (y)
                    avl_set_parent(y, x_parent);
                __avl_rotate_set_parents(x_parent, x, root);
            }
            return;
        } else if (bf == AVL_BALANCED) {
            avl_set_bf(x_parent, ((x == x_parent->avl_left) ?
                                 AVL_LEFT_HEAVY : AVL_RIGHT_HEAVY));
            x = x_parent;
        } else {
            avl_set_bf(x_parent, AVL_BALANCED);
            if (x == x_parent->avl_left) {
                y = x->avl_right;
                bf = avl_bf(x);
                avl_set_bf(x, AVL_BALANCED);
                if (bf == AVL_RIGHT_HEAVY) {
                    bf = avl_bf(y);
                    avl_set_bf(y, AVL_BALANCED);
                    if (bf == AVL_LEFT_HEAVY)
                        avl_set_bf(x_parent, AVL_RIGHT_HEAVY);
                    else if (bf == AVL_RIGHT_HEAVY)
                        avl_set_bf(x, AVL_LEFT_HEAVY);
                    x->avl_right = tmp = y->avl_left;
                    y->avl_left = x;
                    if (tmp)
                        avl_set_parent(tmp, x);
                    avl_set_parent(x, y);
                    x = y;
                    y = x->avl_right;
                }
                x_parent->avl_left = y;
                x->avl_right = x_parent;
                if (y)
                    avl_set_parent(y, x_parent);
                __avl_rotate_set_parents(x_parent, x, root);
            }
            return;
        }
    }
}

static void __avl_erase_fixup(struct avl_node *x, struct avl_node *x_parent,
                              struct avl_root *root)
{
    struct avl_node *y, *z, *tmp;
    unsigned long bf, bf1;
    for ( ; x_parent; x_parent = avl_parent(x)) {
        bf = avl_bf(x_parent);
        if (bf == AVL_RIGHT_HEAVY) {
            avl_set_bf(x_parent, AVL_BALANCED);
            if (x == x_parent->avl_right)
                x = x_parent;
            else {
                y = x_parent->avl_right;
                z = y->avl_left;
                bf = avl_bf(y);
                avl_set_bf(y, AVL_BALANCED);
                if (bf == AVL_LEFT_HEAVY) {
                    bf1 = avl_bf(z);
                    avl_set_bf(z, AVL_BALANCED);
                    if (bf1 == AVL_RIGHT_HEAVY)
                        avl_set_bf(x_parent, AVL_LEFT_HEAVY);
                    else if (bf1 == AVL_LEFT_HEAVY)
                        avl_set_bf(y, AVL_RIGHT_HEAVY);
                    y->avl_left = tmp = z->avl_right;
                    z->avl_right = y;
                    if (tmp)
                        avl_set_parent(tmp, y);
                    avl_set_parent(y, z);
                    y = z;
                    z = y->avl_left;
                } else {
                    if (bf == AVL_BALANCED) {
                        avl_set_bf(x_parent, AVL_RIGHT_HEAVY);
                        avl_set_bf(y, AVL_LEFT_HEAVY);
                    }
                }
                y->avl_left = x_parent;
                x_parent->avl_right = z;
                if (z)
                    avl_set_parent(z, x_parent);
                __avl_rotate_set_parents(x_parent, y, root);
                if (bf == AVL_BALANCED)
                    return;
                x = y;
            }
        } else if (bf == AVL_BALANCED) {
            avl_set_bf(x_parent, ((x == x_parent->avl_left) ?
                                 AVL_RIGHT_HEAVY : AVL_LEFT_HEAVY));
            return;
        } else {
            avl_set_bf(x_parent, AVL_BALANCED);
            if (x == x_parent->avl_left)
                x = x_parent;
            else {
                y = x_parent->avl_left;
                z = y->avl_right;
                bf = avl_bf(y);
                avl_set_bf(y, AVL_BALANCED);
                if (bf == AVL_RIGHT_HEAVY) {
                    bf1 = avl_bf(z);
                    if (bf1 == AVL_LEFT_HEAVY)
                        avl_set_bf(x_parent, AVL_RIGHT_HEAVY);
                    else if (bf1 == AVL_RIGHT_HEAVY)
                        avl_set_bf(y, AVL_LEFT_HEAVY);
                    avl_set_bf(z, AVL_BALANCED);
                    y->avl_right = tmp = z->avl_left;
                    z->avl_left = y;
                    if (tmp)
                        avl_set_parent(tmp, y);
                    avl_set_parent(y, z);
                    y = z;
                    z = y->avl_right;
                } else {
                    if (bf == AVL_BALANCED) {
                        avl_set_bf(x_parent, AVL_LEFT_HEAVY);
                        avl_set_bf(y, AVL_RIGHT_HEAVY);
                    }
                }
                y->avl_right = x_parent;
                x_parent->avl_left = z;
                if (z)
                    avl_set_parent(z, x_parent);
                __avl_rotate_set_parents(x_parent, y, root);
                if (bf == AVL_BALANCED)
                    return;
                x = y;
            }
        }
    }
}

void avl_erase(struct avl_node *node, struct avl_root *root)
{
    struct avl_node *child = node->avl_left, *tmp = node->avl_right;
    struct avl_node *parent, *parent1 = avl_parent(node);
    if (!child || !tmp) {
        tmp = child = (!tmp ? child : tmp);
        parent = parent1;
    } else {
        parent = tmp;
        while (tmp->avl_left)
            tmp = tmp->avl_left;
        tmp->avl_left = child;
        avl_set_parent(child, tmp);
        child = tmp->avl_right;
        if (parent != tmp) {
            avl_set_parent(parent, tmp);
            tmp->avl_right = parent;
            parent = avl_parent(tmp);
            parent->avl_left = child;
        }
        tmp->__avl_parent_bf = node->__avl_parent_bf;
    }
    __avl_change_child(node, tmp, parent1, root);
    if (child)
        avl_set_parent(child, parent);
    __avl_erase_fixup(child, parent, root);
}

struct avl_node *avl_first(const struct avl_root *root)
{
    struct avl_node *n;

    n = root->avl_node;
    if (!n)
        return NULL;
    while (n->avl_left)
        n = n->avl_left;
    return n;
}

struct avl_node *avl_last(const struct avl_root *root)
{
    struct avl_node *n;

    n = root->avl_node;
    if (!n)
        return NULL;
    while (n->avl_right)
        n = n->avl_right;
    return n;
}

struct avl_node *avl_next(const struct avl_node *node)
{
    struct avl_node *parent;

    if (avl_parent(node) == node)
        return NULL;

    if (node->avl_right) {
        node = node->avl_right;
        while (node->avl_left)
            node = node->avl_left;
        return (struct avl_node *)node;
    }

    while ((parent = avl_parent(node)) && node == parent->avl_right)
        node = parent;

    return parent;
}

struct avl_node *avl_prev(const struct avl_node *node)
{
    struct avl_node *parent;

    if (avl_parent(node) == node)
        return NULL;

    if (node->avl_left) {
        node = node->avl_left;
        while (node->avl_right)
            node = node->avl_right;
        return (struct avl_node *)node;
    }

    while ((parent = avl_parent(node)) && node == parent->avl_left)
        node = parent;

    return parent;
}

void avl_replace_node(struct avl_node *victim, struct avl_node *newnode,
                      struct avl_root *root)
{
    struct avl_node *parent = avl_parent(victim);
    __avl_change_child(victim, newnode, parent, root);
    if (victim->avl_left)
        avl_set_parent(victim->avl_left, newnode);
    if (victim->avl_right)
        avl_set_parent(victim->avl_right, newnode);

    *newnode = *victim;
}
