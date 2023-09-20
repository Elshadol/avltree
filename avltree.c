#include "avltree.h"

static void __avl_rotate_left(struct avl_node *x, struct avl_root *root)
{
    struct avl_node *y = x->avl_right;
    if ((x->avl_right = y->avl_left))
        y->avl_left->avl_parent = x;
    if ((y->avl_parent = x->avl_parent)) {
        if (x == x->avl_parent->avl_left)
            x->avl_parent->avl_left = y;
        else
            x->avl_parent->avl_right = y;
    } else
        root->avl_node = y;
    y->avl_left = x;
    x->avl_parent = y;
}

static void __avl_rotate_right(struct avl_node *x, struct avl_root *root)
{
    struct avl_node *y = x->avl_left;
    if ((x->avl_left = y->avl_right))
        y->avl_right->avl_parent = x;
    if ((y->avl_parent = x->avl_parent)) {
        if (x == x->avl_parent->avl_right)
            x->avl_parent->avl_right = y;
        else
            x->avl_parent->avl_left = y;
    } else
        root->avl_node = y;
    y->avl_right = x;
    x->avl_parent = y;
}

static void __avl_rotate_left_right(struct avl_node *x, struct avl_root *root)
{
    struct avl_node *y = x->avl_left;
    struct avl_node *z = y->avl_right;
    if ((x->avl_left = z->avl_right))
        z->avl_right->avl_parent = x;
    if ((y->avl_right = z->avl_left))
        z->avl_left->avl_parent = y;
    if ((z->avl_parent = x->avl_parent)) {
        if (x == x->avl_parent->avl_right)
            x->avl_parent->avl_right = z;
        else
            x->avl_parent->avl_left= z;
    } else
        root->avl_node = z;
    z->avl_left = y;
    y->avl_parent = z;
    z->avl_right = x;
    x->avl_parent = z;
}

static void __avl_rotate_right_left(struct avl_node *x, struct avl_root *root)
{
    struct avl_node *y = x->avl_right;
    struct avl_node *z = y->avl_left;
    if ((x->avl_right = z->avl_left))
        z->avl_left->avl_parent = x;
    if ((y->avl_left = z->avl_right))
        z->avl_right->avl_parent = y;
    if ((z->avl_parent = x->avl_parent)) {
        if (x == x->avl_parent->avl_left)
            x->avl_parent->avl_left = z;
        else
            x->avl_parent->avl_right = z;
    } else
        root->avl_node = z;
    z->avl_left = x;
    x->avl_parent = z;
    z->avl_right = y;
    y->avl_parent = z;
}

void avl_insert_fixup(struct avl_node *node, struct avl_root *root)
{
    struct avl_node *parent, *child;
    while ((parent = node->avl_parent)) {
        if (parent->avl_bf == AVL_BALANCED) {
            parent->avl_bf =
                (node == parent->avl_left) ? AVL_LEFT_HEAVY : AVL_RIGHT_HEAVY;
            node = parent;
        } else if (parent->avl_bf == AVL_LEFT_HEAVY) {
            parent->avl_bf = AVL_BALANCED;
            if (node == parent->avl_left) {
                if (node->avl_bf == AVL_LEFT_HEAVY) {
                    node->avl_bf = AVL_BALANCED;
                    __avl_rotate_right(parent, root);
                } else {
                    node->avl_bf = AVL_BALANCED;
                    child = node->avl_right;
                    if (child->avl_bf == AVL_LEFT_HEAVY)
                        parent->avl_bf = AVL_RIGHT_HEAVY;
                    else if (child->avl_bf == AVL_RIGHT_HEAVY)
                        node->avl_bf = AVL_LEFT_HEAVY;
                    child->avl_bf = AVL_BALANCED;
                    __avl_rotate_left_right(parent, root);
                }
            }
            return;
        } else {
            parent->avl_bf = AVL_BALANCED;
            if (node == parent->avl_right) {
                if (node->avl_bf == AVL_RIGHT_HEAVY) {
                    node->avl_bf = AVL_BALANCED;
                    __avl_rotate_left(parent, root);
                } else {
                    node->avl_bf = AVL_BALANCED;
                    child = node->avl_left;
                    if (child->avl_bf == AVL_RIGHT_HEAVY)
                        parent->avl_bf = AVL_LEFT_HEAVY;
                    else if (child->avl_bf == AVL_LEFT_HEAVY)
                        node->avl_bf = AVL_RIGHT_HEAVY;
                    child->avl_bf = AVL_BALANCED;
                    __avl_rotate_right_left(parent, root);
                }
            }
            return;
        }
    }
}

static void __avl_erase_fixup(struct avl_node *node, struct avl_node *parent,
                              struct avl_root *root)
{
    struct avl_node *sibling, *nephew;
    for ( ; parent; parent = node->avl_parent) {
        if (parent->avl_bf == AVL_BALANCED) {
            parent->avl_bf =
                (node == parent->avl_left) ? AVL_RIGHT_HEAVY : AVL_LEFT_HEAVY;
            return;
        } else if (parent->avl_bf == AVL_LEFT_HEAVY) {
            parent->avl_bf = AVL_BALANCED;
            if (node == parent->avl_left) 
                node = parent;
            else {
                sibling = parent->avl_left;
                if (sibling->avl_bf == AVL_RIGHT_HEAVY) {
                    nephew = sibling->avl_right;
                    sibling->avl_bf = AVL_BALANCED;
                    if (nephew->avl_bf == AVL_LEFT_HEAVY)
                        parent->avl_bf = AVL_RIGHT_HEAVY;
                    else if (nephew->avl_bf == AVL_RIGHT_HEAVY)
                        sibling->avl_bf = AVL_LEFT_HEAVY;
                    nephew->avl_bf = AVL_BALANCED;
                    __avl_rotate_left_right(parent, root);
                } else {
                    if (sibling->avl_bf == AVL_BALANCED) {
                        parent->avl_bf = AVL_LEFT_HEAVY;
                        sibling->avl_bf = AVL_RIGHT_HEAVY;
                    } else
                        sibling->avl_bf = AVL_BALANCED;
                    __avl_rotate_right(parent, root);
                    if (parent->avl_bf == AVL_LEFT_HEAVY)
                        return;
                }
                node = parent->avl_parent;
            }
        } else {
            parent->avl_bf = AVL_BALANCED;
            if (node == parent->avl_right) 
                node = parent;
            else {
                sibling = parent->avl_right;
                if (sibling->avl_bf == AVL_LEFT_HEAVY) {
                    nephew = sibling->avl_left;
                    sibling->avl_bf = AVL_BALANCED;
                    if (nephew->avl_bf == AVL_LEFT_HEAVY)
                        sibling->avl_bf = AVL_RIGHT_HEAVY;
                    else if (nephew->avl_bf == AVL_RIGHT_HEAVY)
                        parent->avl_bf = AVL_LEFT_HEAVY;
                    nephew->avl_bf = AVL_BALANCED;
                    __avl_rotate_right_left(parent, root);
                } else {
                    if (sibling->avl_bf == AVL_BALANCED) {
                        parent->avl_bf = AVL_RIGHT_HEAVY;
                        sibling->avl_bf = AVL_LEFT_HEAVY;
                    } else
                        sibling->avl_bf = AVL_BALANCED;
                    __avl_rotate_left(parent, root);
                    if (parent->avl_bf == AVL_RIGHT_HEAVY)
                        return;
                }
                node = parent->avl_parent;
            }
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
        struct avl_node *old = node, *other;
        node = node->avl_right;
        while ((other = node->avl_left))
            node = other;
        child = node->avl_right;
        node->avl_left = old->avl_left;
        old->avl_left->avl_parent = node;
        if (node != old->avl_right) {
            parent = node->avl_parent;
            if (child)
                child->avl_parent = parent;
            parent->avl_left = child;
            node->avl_right = old->avl_right;
            old->avl_right->avl_parent = node;
        } else
            parent = node;
        if (old->avl_parent) {
            if (old == old->avl_parent->avl_left)
                old->avl_parent->avl_left = node;
            else
                old->avl_parent->avl_right = node;
        } else
            root->avl_node = node;
        node->avl_parent = old->avl_parent;
        node->avl_bf = old->avl_bf;
        goto ERASE_FIXUP;
    }
    parent = node->avl_parent;
    if (child)
        child->avl_parent = parent;
    if (parent) {
        if (parent->avl_left == node)
            parent->avl_left = child;
        else
            parent->avl_right = child;
    } else
        root->avl_node = child;

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

    if (node->avl_parent == node)
        return NULL;

    if (node->avl_right) {
        node = node->avl_right;
        while (node->avl_left)
            node = node->avl_left;
        return (struct avl_node *)node;
    }

    while ((parent = (node)->avl_parent) && node == parent->avl_right)
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

    while ((parent = (node)->avl_parent) && node == parent->avl_left)
        node = parent;

    return parent;
}

void avl_replace_node(struct avl_node *victim, struct avl_node *newnode,
                      struct avl_root *root)
{
    struct avl_node *parent = (victim)->avl_parent;

    if (parent) {
        if (victim == parent->avl_left)
            parent->avl_left = newnode;
        else
            parent->avl_right = newnode;
    } else {
        root->avl_node = newnode;
    }
    if (victim->avl_left)
        victim->avl_left->avl_parent = newnode;
    if (victim->avl_right)
        victim->avl_right->avl_parent = newnode;

    *newnode = *victim;
}
