#include "avltree.h"

static inline void __avl_change_child(struct avl_node *x, struct avl_node *y,
                                      struct avl_node *x_parent,
                                      struct avl_root *root)
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
    struct avl_node *x_parent, *tmp;
    while ((x_parent = avl_parent(x))) {
        switch (avl_bf(x_parent)) {
        case AVL_RIGHT_HEAVY:
            avl_set_bf(x_parent, AVL_BALANCED);
            if (x == x_parent->avl_right) {
                if (avl_left_heavy(x)) {
                    struct avl_node *y = x->avl_left;
                    avl_set_bf(x, AVL_BALANCED);
                    switch (avl_bf(y)) {
                    case AVL_LEFT_HEAVY:
                        avl_set_bf(x, AVL_RIGHT_HEAVY);
                        break;
                    case AVL_BALANCED:
                        break;
                    case AVL_RIGHT_HEAVY:
                        avl_set_bf(x_parent, AVL_LEFT_HEAVY);
                        break;
                    }
                    avl_set_bf(y, AVL_BALANCED);
                    x->avl_left = tmp = y->avl_right;
                    y->avl_right = x;
                    if (tmp)
                        avl_set_parent(tmp, x);
                    avl_set_parent(x, y);
                    x = y;
                } else
                    avl_set_bf(x, AVL_BALANCED);
                x_parent->avl_right = tmp = x->avl_left;
                x->avl_left = x_parent;
                if (tmp)
                    avl_set_parent(tmp, x_parent);
                __avl_rotate_set_parents(x_parent, x, root);
            }
            return;

        case AVL_BALANCED:
            avl_set_bf(x_parent, ((x == x_parent->avl_left) ?
                                  AVL_LEFT_HEAVY : AVL_RIGHT_HEAVY));
            x = x_parent;
            break;

        case AVL_LEFT_HEAVY:
            avl_set_bf(x_parent, AVL_BALANCED);
            if (x == x_parent->avl_left) {
                if (avl_right_heavy(x)) {
                    struct avl_node *y = x->avl_right;
                    avl_set_bf(x, AVL_BALANCED);
                    switch (avl_bf(y)) {
                    case AVL_LEFT_HEAVY:
                        avl_set_bf(x_parent, AVL_RIGHT_HEAVY);
                        break;
                    case AVL_BALANCED:
                        break;
                    case AVL_RIGHT_HEAVY:
                        avl_set_bf(x, AVL_LEFT_HEAVY);
                        break;
                    }
                    avl_set_bf(y, AVL_BALANCED);
                    x->avl_right = tmp = y->avl_left;
                    y->avl_left = x;
                    if (tmp)
                        avl_set_parent(tmp, x);
                    avl_set_parent(x, y);
                    x = y;
                } else
                    avl_set_bf(x, AVL_BALANCED);
                x_parent->avl_left = tmp = x->avl_right;
                x->avl_right = x_parent;
                if (tmp)
                    avl_set_parent(tmp, x_parent);
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
    for ( ; x_parent; x_parent = avl_parent(x)) {
        switch (avl_bf(x_parent)) {
        case AVL_RIGHT_HEAVY:
            avl_set_bf(x_parent, AVL_BALANCED);
            if (x == x_parent->avl_right)
                x = x_parent;
            else {
                y = x_parent->avl_right;
                if (avl_left_heavy(y)) {
                    z = y->avl_left;
                    avl_set_bf(y, AVL_BALANCED);
                    switch (avl_bf(z)) {
                    case AVL_RIGHT_HEAVY:
                        avl_set_bf(x_parent, AVL_LEFT_HEAVY);
                        break;
                    case AVL_BALANCED:
                        break;
                    case AVL_LEFT_HEAVY:
                        avl_set_bf(y, AVL_RIGHT_HEAVY);
                        break;
                    }
                    avl_set_bf(z, AVL_BALANCED);
                    y->avl_left = tmp = z->avl_right;
                    z->avl_right = y;
                    if (tmp)
                        avl_set_parent(tmp, y);
                    avl_set_parent(y, z);
                    y = z;
                } else {
                    if (avl_balanced(y)) {
                        avl_set_bf(x_parent, AVL_RIGHT_HEAVY);
                        avl_set_bf(y, AVL_LEFT_HEAVY);
                    } else
                        avl_set_bf(y, AVL_BALANCED);
                }
                x_parent->avl_right = tmp = y->avl_left;
                y->avl_left = x_parent;
                if (tmp)
                    avl_set_parent(tmp, x_parent);
                __avl_rotate_set_parents(x_parent, y, root);
                x = y;
                if (avl_left_heavy(x))
                    return;
            }
            break;

        case AVL_BALANCED:
            avl_set_bf(x_parent, ((x == x_parent->avl_left) ?
                                  AVL_RIGHT_HEAVY : AVL_LEFT_HEAVY));
            return;

        case AVL_LEFT_HEAVY:
            avl_set_bf(x_parent, AVL_BALANCED);
            if (x == x_parent->avl_left)
                x = x_parent;
            else {
                y = x_parent->avl_left;
                if (avl_right_heavy(y)) {
                    z = y->avl_right;
                    avl_set_bf(y, AVL_BALANCED);
                    switch (avl_bf(z)) {
                    case AVL_LEFT_HEAVY:
                        avl_set_bf(x_parent, AVL_RIGHT_HEAVY);
                        break;
                    case AVL_BALANCED:
                        break;
                    case AVL_RIGHT_HEAVY:
                        avl_set_bf(y, AVL_LEFT_HEAVY);
                        break;
                    }
                    avl_set_bf(z, AVL_BALANCED);
                    y->avl_right = tmp = z->avl_left;
                    z->avl_left = y;
                    if (tmp)
                        avl_set_parent(tmp, y);
                    avl_set_parent(y, z);
                    y = z;
                } else {
                    if (avl_balanced(y)) {
                        avl_set_bf(x_parent, AVL_LEFT_HEAVY);
                        avl_set_bf(y, AVL_RIGHT_HEAVY);
                    } else
                        avl_set_bf(y, AVL_BALANCED);
                }
                x_parent->avl_left = tmp = y->avl_right;
                y->avl_right = x_parent;
                if (tmp)
                    avl_set_parent(tmp, x_parent);
                __avl_rotate_set_parents(x_parent, y, root);
                x = y;
                if (avl_right_heavy(x))
                    return;
            }
            break;
        }
    }
}

void avl_erase(struct avl_node *node, struct avl_root *root)
{
    struct avl_node *child = node->avl_left,
                         *tmp = node->avl_right,
                          *parent = NULL;
    unsigned long tmp1 = node->__avl_parent_bf;
    if (!child || !tmp) {
        tmp = child = (!tmp ? child : tmp);
        tmp1 = (tmp1 & ~3);
        parent = (struct avl_node *)tmp1;
    } else {
        parent = tmp;
        while (tmp->avl_left != NULL)
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
        tmp->__avl_parent_bf = tmp1;
        tmp1 = (tmp1 & ~3);
    }
    __avl_change_child(node, tmp, (struct avl_node *)tmp1, root);
    if (child)
        avl_set_parent(child, parent);
    if (parent)
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
