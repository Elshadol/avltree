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

static void __avl_rotate_left(struct avl_node *x, struct avl_root *root)
{
    struct avl_node *y = x->avl_right, *tmp;
    x->avl_right = tmp = y->avl_left;
    y->avl_left = x;
    if (tmp)
        avl_set_parent(tmp, x);
    __avl_rotate_set_parents(x, y, root);
}

static void __avl_rotate_right(struct avl_node *x, struct avl_root *root)
{
    struct avl_node *y = x->avl_left, *tmp;
    x->avl_left = tmp = y->avl_right;
    y->avl_right = x;
    if (tmp)
        avl_set_parent(tmp, x);
    __avl_rotate_set_parents(x, y, root);
}

static void __avl_rotate_left_right(struct avl_node *x, struct avl_root *root)
{
    struct avl_node *y = x->avl_left, *tmp;
    struct avl_node *z = y->avl_right;
    x->avl_left = tmp = z->avl_right;
    z->avl_right = x;
    if (tmp)
        avl_set_parent(tmp, x);
    y->avl_right = tmp = z->avl_left;
    z->avl_left = y;
    if (tmp)
        avl_set_parent(tmp, y);
    avl_set_parent(y, z);
    __avl_rotate_set_parents(x, z, root);
}

static void __avl_rotate_right_left(struct avl_node *x, struct avl_root *root)
{
    struct avl_node *y = x->avl_right, *tmp;
    struct avl_node *z = y->avl_left;
    x->avl_right = tmp = z->avl_left;
    z->avl_left = x;
    if (tmp)
        avl_set_parent(tmp, x);
    y->avl_left = tmp = z->avl_right;
    z->avl_right = y;
    if (tmp)
        avl_set_parent(tmp, y);
    avl_set_parent(y, z);
    __avl_rotate_set_parents(x, z, root);
}

static void __avl_insert_fixup_r(struct avl_node *x,
                                 struct avl_node *x_parent,
                                 struct avl_root *root)
{
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
        __avl_rotate_right_left(x_parent, root);
    } else {
        avl_set_bf(x, AVL_BALANCED);
        __avl_rotate_left(x_parent, root);
    }
}

static  void __avl_insert_fixup_l(struct avl_node *x,
                                  struct avl_node *x_parent,
                                  struct avl_root *root)
{
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
        __avl_rotate_left_right(x_parent, root);
    } else {
        avl_set_bf(x, AVL_BALANCED);
        __avl_rotate_right(x_parent, root);
    }
}

void avl_insert_fixup(struct avl_node *x, struct avl_root *root)
{
    struct avl_node *x_parent;
    while ((x_parent = avl_parent(x))) {
        switch (avl_bf(x_parent)) {
        case AVL_RIGHT_HEAVY:
            avl_set_bf(x_parent, AVL_BALANCED);
            if (x == x_parent->avl_right)
                __avl_insert_fixup_r(x, x_parent, root);
            return;

        case AVL_BALANCED:
            avl_set_bf(x_parent, ((x == x_parent->avl_left) ?
                                  AVL_LEFT_HEAVY : AVL_RIGHT_HEAVY));
            x = x_parent;
            break;

        case AVL_LEFT_HEAVY:
            avl_set_bf(x_parent, AVL_BALANCED);
            if (x == x_parent->avl_left)
                __avl_insert_fixup_l(x, x_parent, root);
            return;
        }
    }
}

static struct avl_node * __avl_erase_fixup_l(struct avl_node *x_parent,
        struct avl_root *root)
{
    struct avl_node *y = x_parent->avl_left;
    if (avl_right_heavy(y)) {
        struct avl_node *z = y->avl_right;
        avl_set_bf(y, AVL_BALANCED);
        switch (avl_bf(z)) {
        case AVL_RIGHT_HEAVY:
            avl_set_bf(y, AVL_LEFT_HEAVY);
            break;
        case AVL_BALANCED:
            break;
        case AVL_LEFT_HEAVY:
            avl_set_bf(x_parent, AVL_RIGHT_HEAVY);
            break;
        }
        avl_set_bf(z, AVL_BALANCED);
        __avl_rotate_left_right(x_parent, root);
        return z;
    } else {
        if (avl_balanced(y)) {
            avl_set_bf(x_parent, AVL_LEFT_HEAVY);
            avl_set_bf(y, AVL_RIGHT_HEAVY);
        } else
            avl_set_bf(y, AVL_BALANCED);
        __avl_rotate_right(x_parent, root);
        return y;
    }
}

static struct avl_node * __avl_erase_fixup_r( struct avl_node *x_parent,
        struct avl_root *root)
{
    struct avl_node *y = x_parent->avl_right;
    if (avl_left_heavy(y)) {
        struct avl_node *z = y->avl_left;
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
        __avl_rotate_right_left(x_parent, root);
        return z;
    } else {
        if (avl_balanced(y)) {
            avl_set_bf(x_parent, AVL_RIGHT_HEAVY);
            avl_set_bf(y, AVL_LEFT_HEAVY);
        } else
            avl_set_bf(y, AVL_BALANCED);
        __avl_rotate_left(x_parent, root);
        return y;
    }
}

static void __avl_erase_fixup(struct avl_node *x, struct avl_node *x_parent,
                              struct avl_root *root)
{
    for ( ; x_parent; x_parent = avl_parent(x)) {
        switch (avl_bf(x_parent)) {
        case AVL_RIGHT_HEAVY:
            avl_set_bf(x_parent, AVL_BALANCED);
            if (x == x_parent->avl_right)
                x = x_parent;
            else {
                x = __avl_erase_fixup_r(x_parent, root);
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
                x = __avl_erase_fixup_l(x_parent, root);
                if (avl_right_heavy(x))
                    return;
            }
            break;
        }
    }
}

void avl_erase(struct avl_node *node, struct avl_root *root)
{
    struct avl_node *child, *parent;
    if (!node->avl_left)
        child = node->avl_right;
    else if (!node->avl_right)
        child = node->avl_left;
    else {
        struct avl_node *old = node, *tmp;
        node = node->avl_right;
        while ((tmp = node->avl_left))
            node = tmp;
        avl_set_parent(old->avl_left, node);
        child = node->avl_right;
        if (node != old->avl_right) {
            node->avl_right = old->avl_right;
            avl_set_parent(old->avl_right, node);
            parent = avl_parent(node);
            parent->avl_left = child;
            if (child)
                avl_set_parent(child, parent);
        } else
            parent = node;
        __avl_change_child(old, node, avl_parent(old), root);
        avl_set_parent(node, avl_parent(old));
        avl_set_bf(node, avl_bf(old));
        goto ERASE_FIXUP;
    }
    parent = avl_parent(node);
    __avl_change_child(node, child, parent, root);
    if (child)
        avl_set_parent(child, parent);

ERASE_FIXUP:
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
