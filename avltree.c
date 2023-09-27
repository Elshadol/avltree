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
    struct avl_node *x_parent = x->avl_parent;
    x->avl_parent = y;
    y->avl_parent = x_parent;
    __avl_change_child(x, y, x_parent, root);
}

static void __avl_rotate_left(struct avl_node *x, struct avl_root *root)
{
    struct avl_node *y = x->avl_right;
    if ((x->avl_right = y->avl_left))
        y->avl_left->avl_parent = x;
    y->avl_left = x;
    __avl_rotate_set_parents(x, y, root);
}

static void __avl_rotate_right(struct avl_node *x, struct avl_root *root)
{
    struct avl_node *y = x->avl_left;
    if ((x->avl_left = y->avl_right))
        y->avl_right->avl_parent = x;
    y->avl_right = x;
    __avl_rotate_set_parents(x, y, root);
}

static void __avl_rotate_left_right(struct avl_node *x, struct avl_root *root)
{
    struct avl_node *y = x->avl_left;
    struct avl_node *z = y->avl_right;
    if ((x->avl_left = z->avl_right))
        z->avl_right->avl_parent = x;
    z->avl_right = x;
    if ((y->avl_right = z->avl_left))
        z->avl_left->avl_parent = y;
    z->avl_left = y;
    y->avl_parent = z;
    __avl_rotate_set_parents(x, z, root);
}

static void __avl_rotate_right_left(struct avl_node *x, struct avl_root *root)
{
    struct avl_node *y = x->avl_right;
    struct avl_node *z = y->avl_left;
    if ((x->avl_right = z->avl_left))
        z->avl_left->avl_parent = x;
    z->avl_left = x;
    if ((y->avl_left = z->avl_right))
        z->avl_right->avl_parent = y;
    z->avl_right = y;
    y->avl_parent = z;
    __avl_rotate_set_parents(x, z, root);
}

static void __avl_insert_fixup_r(struct avl_node *x,
                                 struct avl_node *x_parent,
                                 struct avl_root *root)
{
    if (x->avl_bf == AVL_LEFT_HEAVY) {
        x->avl_bf = AVL_BALANCED;
        struct avl_node *y = x->avl_left;
        switch (y->avl_bf) {
        case AVL_LEFT_HEAVY:
            x->avl_bf = AVL_RIGHT_HEAVY;
            break;
        case AVL_BALANCED:
            break;
        case AVL_RIGHT_HEAVY:
            x_parent->avl_bf = AVL_LEFT_HEAVY;
            break;
        }
        y->avl_bf = AVL_BALANCED;
        __avl_rotate_right_left(x_parent, root);
    } else {
        x->avl_bf = AVL_BALANCED;
        __avl_rotate_left(x_parent, root);
    }
}

static  void __avl_insert_fixup_l(struct avl_node *x,
                                  struct avl_node *x_parent,
                                  struct avl_root *root)
{
    if (x->avl_bf == AVL_RIGHT_HEAVY) {
        x->avl_bf = AVL_BALANCED;
        struct avl_node *y = x->avl_right;
        switch (y->avl_bf) {
        case AVL_LEFT_HEAVY:
            x_parent->avl_bf = AVL_RIGHT_HEAVY;
            break;
        case AVL_BALANCED:
            break;
        case AVL_RIGHT_HEAVY:
            x->avl_bf = AVL_LEFT_HEAVY;
            break;
        }
        y->avl_bf = AVL_BALANCED;
        __avl_rotate_left_right(x_parent, root);
    } else {
        x->avl_bf = AVL_BALANCED;
        __avl_rotate_right(x_parent, root);
    }
}

void avl_insert_fixup(struct avl_node *x, struct avl_root *root)
{
    struct avl_node *x_parent;
    while ((x_parent = x->avl_parent)) {
        switch (x_parent->avl_bf) {
        case AVL_RIGHT_HEAVY:
            x_parent->avl_bf = AVL_BALANCED;
            if (x == x_parent->avl_right)
                __avl_insert_fixup_r(x, x_parent, root);
            return;

        case AVL_BALANCED:
            x_parent->avl_bf = (x == x_parent->avl_left) ?
                               AVL_LEFT_HEAVY : AVL_RIGHT_HEAVY;
            x = x_parent;
            break;

        case AVL_LEFT_HEAVY:
            x_parent->avl_bf = AVL_BALANCED;
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
    if (y->avl_bf == AVL_RIGHT_HEAVY) {
        y->avl_bf = AVL_BALANCED;
        struct avl_node *z = y->avl_right;
        switch (z->avl_bf) {
        case AVL_RIGHT_HEAVY:
            y->avl_bf = AVL_LEFT_HEAVY;
            break;
        case AVL_BALANCED:
            break;
        case AVL_LEFT_HEAVY:
            x_parent->avl_bf = AVL_RIGHT_HEAVY;
            break;
        }
        z->avl_bf = AVL_BALANCED;
        __avl_rotate_left_right(x_parent, root);
        return z;
    } else {
        if (y->avl_bf == AVL_BALANCED) {
            x_parent->avl_bf = AVL_LEFT_HEAVY;
            y->avl_bf = AVL_RIGHT_HEAVY;
        } else
            y->avl_bf = AVL_BALANCED;
        __avl_rotate_right(x_parent, root);
        return y;
    }
}

static struct avl_node * __avl_erase_fixup_r( struct avl_node *x_parent,
        struct avl_root *root)
{
    struct avl_node *y = x_parent->avl_right;
    if (y->avl_bf == AVL_LEFT_HEAVY) {
        y->avl_bf = AVL_BALANCED;
        struct avl_node *z = y->avl_left;
        switch (z->avl_bf) {
        case AVL_RIGHT_HEAVY:
            x_parent->avl_bf = AVL_LEFT_HEAVY;
            break;
        case AVL_BALANCED:
            break;
        case AVL_LEFT_HEAVY:
            y->avl_bf = AVL_RIGHT_HEAVY;
            break;
        }
        z->avl_bf = AVL_BALANCED;
        __avl_rotate_right_left(x_parent, root);
        return z;
    } else {
        if (y->avl_bf == AVL_BALANCED) {
            x_parent->avl_bf = AVL_RIGHT_HEAVY;
            y->avl_bf = AVL_LEFT_HEAVY;
        } else
            y->avl_bf = AVL_BALANCED;
        __avl_rotate_left(x_parent, root);
        return y;
    }
}

static void __avl_erase_fixup(struct avl_node *x, struct avl_node *x_parent,
                              struct avl_root *root)
{
    for ( ; x_parent; x_parent = x->avl_parent) {
        switch (x_parent->avl_bf) {
        case AVL_RIGHT_HEAVY:
            x_parent->avl_bf = AVL_BALANCED;
            if (x == x_parent->avl_right)
                x = x_parent;
            else {
                x = __avl_erase_fixup_r(x_parent, root);
                if (x->avl_bf == AVL_LEFT_HEAVY)
                    return;
            }
            break;

        case AVL_BALANCED:
            x_parent->avl_bf = (x == x_parent->avl_left) ?
                               AVL_RIGHT_HEAVY : AVL_LEFT_HEAVY;
            return;

        case AVL_LEFT_HEAVY:
            x_parent->avl_bf = AVL_BALANCED;
            if (x == x_parent->avl_left)
                x = x_parent;
            else {
                x = __avl_erase_fixup_l(x_parent, root);
                if (x->avl_bf == AVL_RIGHT_HEAVY)
                    return;
            }
            break;
        }
    }
}

void avl_erase(struct avl_node *node, struct avl_root *root)
{
    struct avl_node *child, *parent;
    if (node->avl_left && node->avl_right) {
        struct avl_node *old = node, *tmp;
        node = node->avl_right;
        while ((tmp = node->avl_left))
            node = tmp;
        node->avl_left = old->avl_left;
        old->avl_left->avl_parent = node;
        child = node->avl_right;
        if (node != old->avl_right) {
            node->avl_right = old->avl_right;
            old->avl_right->avl_parent = node;
            parent = node->avl_parent;
            parent->avl_left = child;
            if (child)
                child->avl_parent = parent;
        } else
            parent = node;
        __avl_change_child(old, node, old->avl_parent, root);
        node->avl_parent = old->avl_parent;
    } else {
        if (node->avl_left == NULL)
            child = node->avl_right;
        else
            child = node->avl_left;
        parent = node->avl_parent;
        __avl_change_child(node, child, parent, root);
        if (child)
            child->avl_parent = parent;
    }
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

    if (node->avl_parent == node)
        return NULL;

    if (node->avl_right) {
        node = node->avl_right;
        while (node->avl_left)
            node = node->avl_left;
        return (struct avl_node *)node;
    }

    while ((parent = node->avl_parent) && node == parent->avl_right)
        node = parent;

    return parent;
}

struct avl_node *avl_prev(const struct avl_node *node)
{
    struct avl_node *parent;

    if (node->avl_parent == node)
        return NULL;

    if (node->avl_left) {
        node = node->avl_left;
        while (node->avl_right)
            node = node->avl_right;
        return (struct avl_node *)node;
    }

    while ((parent = node->avl_parent) && node == parent->avl_left)
        node = parent;

    return parent;
}

void avl_replace_node(struct avl_node *victim, struct avl_node *newnode,
                      struct avl_root *root)
{
    struct avl_node *parent = (victim)->avl_parent;
    __avl_chang_child(newnode, victim, parent, root);
    if (victim->avl_left)
        victim->avl_left->avl_parent = newnode;
    if (victim->avl_right)
        victim->avl_right->avl_parent = newnode;

    *newnode = *victim;
}
