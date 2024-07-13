#include "avltree.h"

static inline struct avl_node *avl_parent(const struct avl_node *n) {
  return (struct avl_node *)(n->__avl_parent_bf & ~3lu);
}

static inline struct avl_node *avl_bal_parent(const struct avl_node *x) {
  return (struct avl_node *)x->__avl_parent_bf;
}

static inline unsigned long avl_bf(const struct avl_node *n) {
  return (unsigned long)(n->__avl_parent_bf & 3lu);
}

static inline void avl_set_parent(struct avl_node *n, struct avl_node *p) {
  n->__avl_parent_bf = avl_bf(n) + (unsigned long)(p);
}

static inline void avl_set_parent_bf(struct avl_node *n, struct avl_node *p,
                                     unsigned long bf) {
  n->__avl_parent_bf = (unsigned long)p + bf;
}

static inline void avl_set_left_heavy(struct avl_node *n) {
  n->__avl_parent_bf += AVL_LEFT_HEAVY;
}

static inline void avl_set_right_heavy(struct avl_node *n) {
  n->__avl_parent_bf += AVL_RIGHT_HEAVY;
}

static inline void __avl_change_child(struct avl_node *old,
                                      struct avl_node *new,
                                      struct avl_node *parent,
                                      struct avl_root *root) {
  if (parent) {
    if (parent->avl_left == old)
      parent->avl_left = new;
    else
      parent->avl_right = new;
  } else
    root->avl_node = new;
}

static inline void __avl_rotate_set_parent(struct avl_node *old,
                                           struct avl_node *new,
                                           unsigned long oldb,
                                           unsigned long newb,
                                           struct avl_root *root) {
  struct avl_node *parent = avl_parent(old);
  avl_set_parent_bf(old, new, oldb);
  avl_set_parent_bf(new, parent, newb);
  __avl_change_child(old, new, parent, root);
}

void avl_insert_rebalance(struct avl_node *node, struct avl_root *root) {
  unsigned long bf1, bf2, bf3;
  struct avl_node *parent = avl_bal_parent(node), *tmp1, *tmp2;
  while (parent) {
    bf1 = avl_bf(parent);
    tmp1 = parent->avl_right;
    if (node != tmp1) {
      if (bf1 == AVL_RIGHT_HEAVY) {
        node = parent;
        parent = avl_parent(node);
        avl_set_parent_bf(node, parent, AVL_BALANCED);
        break;
      } else if (bf1 == AVL_BALANCED) {
        node = parent;
        parent = avl_bal_parent(node);
        avl_set_parent_bf(node, parent, AVL_LEFT_HEAVY);
        continue;
      } else {
        bf1 = AVL_BALANCED;
        tmp1 = node->avl_right;
        bf2 = avl_bf(node);
        if (bf2 == AVL_RIGHT_HEAVY) {
          bf2 = AVL_BALANCED;
          bf3 = avl_bf(tmp1);
          if (bf3 == AVL_LEFT_HEAVY)
            bf1 = AVL_RIGHT_HEAVY;
          else if (bf3 == AVL_RIGHT_HEAVY)
            bf2 = AVL_LEFT_HEAVY;
          node->avl_right = tmp2 = tmp1->avl_left;
          tmp1->avl_left = node;
          if (tmp2)
            avl_set_parent(tmp2, node);
          avl_set_parent_bf(node, tmp1, bf2);
          node = tmp1;
          tmp1 = node->avl_right;
        }
        bf3 = AVL_BALANCED;
        parent->avl_left = tmp1;
        node->avl_right = parent;
        if (tmp1)
          avl_set_parent(tmp1, parent);
        __avl_rotate_set_parent(parent, node, bf1, bf3, root);
        break;
      }
    } else {
      if (bf1 == AVL_LEFT_HEAVY) {
        node = parent;
        parent = avl_parent(node);
        avl_set_parent_bf(node, parent, AVL_BALANCED);
        break;
      } else if (bf1 == AVL_BALANCED) {
        node = parent;
        parent = avl_bal_parent(node);
        avl_set_parent_bf(node, parent, AVL_RIGHT_HEAVY);
        continue;
      } else {
        bf1 = AVL_BALANCED;
        tmp1 = node->avl_left;
        bf2 = avl_bf(node);
        if (bf2 == AVL_LEFT_HEAVY) {
          bf2 = AVL_BALANCED;
          bf3 = avl_bf(tmp1);
          if (bf3 == AVL_LEFT_HEAVY)
            bf2 = AVL_RIGHT_HEAVY;
          else if (bf3 == AVL_RIGHT_HEAVY)
            bf1 = AVL_LEFT_HEAVY;
          node->avl_left = tmp2 = tmp1->avl_right;
          tmp1->avl_right = node;
          if (tmp2)
            avl_set_parent(tmp2, node);
          avl_set_parent_bf(node, tmp1, bf2);
          node = tmp1;
          tmp1 = node->avl_left;
        }
        bf3 = AVL_BALANCED;
        parent->avl_right = tmp1;
        node->avl_left = parent;
        if (tmp1)
          avl_set_parent(tmp1, parent);
        __avl_rotate_set_parent(parent, node, bf1, bf3, root);
        break;
      }
    }
  }
}

static void __avl_erase_rebalance(struct avl_node *node,
                                  struct avl_node *parent,
                                  struct avl_root *root) {
  unsigned long bf1, bf2, bf3;
  struct avl_node *sibling, *tmp1, *tmp2;

  if (parent->avl_left == parent->avl_right) {
    node = parent;
    parent = avl_parent(node);
    avl_set_parent_bf(node, parent, AVL_BALANCED);
    if (!parent)
      return;
  }

  do {
    bf1 = avl_bf(parent);
    sibling = parent->avl_right;
    if (node != sibling) {
      if (bf1 == AVL_BALANCED) {
        avl_set_right_heavy(parent);
        break;
      } else if (bf1 == AVL_LEFT_HEAVY) {
        node = parent;
        parent = avl_parent(node);
        avl_set_parent_bf(node, parent, AVL_BALANCED);
        continue;
      } else {
        bf1 = AVL_BALANCED;
        bf3 = AVL_BALANCED;
        tmp1 = sibling->avl_left;
        bf2 = avl_bf(sibling);
        if (bf2 == AVL_LEFT_HEAVY) {
          bf2 = AVL_BALANCED;
          bf3 = avl_bf(tmp1);
          if (bf3 == AVL_RIGHT_HEAVY)
            bf1 = AVL_LEFT_HEAVY;
          else if (bf3 == AVL_LEFT_HEAVY)
            bf2 = AVL_RIGHT_HEAVY;
          sibling->avl_left = tmp2 = tmp1->avl_right;
          tmp1->avl_right = sibling;
          if (tmp2)
            avl_set_parent(tmp2, sibling);
          avl_set_parent_bf(sibling, tmp1, bf2);
          sibling = tmp1;
          tmp1 = sibling->avl_left;
          bf3 = AVL_BALANCED;
        } else if (bf2 == AVL_BALANCED) {
          bf1 = AVL_RIGHT_HEAVY;
          bf3 = AVL_LEFT_HEAVY;
        }
        parent->avl_right = tmp1;
        sibling->avl_left = parent;
        if (tmp1)
          avl_set_parent(tmp1, parent);
        __avl_rotate_set_parent(parent, sibling, bf1, bf3, root);
        if (bf1 == AVL_RIGHT_HEAVY)
          break;
        node = sibling;
        parent = avl_bal_parent(node);
        continue;
      }
    } else {
      sibling = parent->avl_left;
      if (bf1 == AVL_BALANCED) {
        avl_set_left_heavy(parent);
        break;
      } else if (bf1 == AVL_RIGHT_HEAVY) {
        node = parent;
        parent = avl_parent(node);
        avl_set_parent_bf(node, parent, AVL_BALANCED);
        continue;
      } else {
        bf1 = AVL_BALANCED;
        bf3 = AVL_BALANCED;
        tmp1 = sibling->avl_right;
        bf2 = avl_bf(sibling);
        if (bf2 == AVL_RIGHT_HEAVY) {
          bf2 = AVL_BALANCED;
          bf3 = avl_bf(tmp1);
          if (bf3 == AVL_LEFT_HEAVY)
            bf1 = AVL_RIGHT_HEAVY;
          else if (bf3 == AVL_RIGHT_HEAVY)
            bf2 = AVL_LEFT_HEAVY;
          sibling->avl_right = tmp2 = tmp1->avl_left;
          tmp1->avl_left = sibling;
          if (tmp2)
            avl_set_parent(tmp2, sibling);
          avl_set_parent_bf(sibling, tmp1, bf2);
          sibling = tmp1;
          tmp1 = sibling->avl_right;
          bf3 = AVL_BALANCED;
        } else if (bf2 == AVL_BALANCED) {
          bf1 = AVL_LEFT_HEAVY;
          bf3 = AVL_RIGHT_HEAVY;
        }
        parent->avl_left = tmp1;
        sibling->avl_right = parent;
        if (tmp1)
          avl_set_parent(tmp1, parent);
        __avl_rotate_set_parent(parent, sibling, bf1, bf3, root);
        if (bf1 == AVL_LEFT_HEAVY)
          break;
        node = sibling;
        parent = avl_bal_parent(node);
        continue;
      }
    }
  } while (parent);
}

void avl_erase(struct avl_node *node, struct avl_root *root) {
  struct avl_node *parent, *parent1 = avl_parent(node);
  struct avl_node *child = node->avl_left, *tmp = node->avl_right;
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
    child->__avl_parent_bf = (unsigned long)parent;
  if (parent)
    __avl_erase_rebalance(child, parent, root);
}

struct avl_node *avl_first(const struct avl_root *root) {
  struct avl_node *n;

  n = root->avl_node;
  if (!n)
    return NULL;
  while (n->avl_left)
    n = n->avl_left;
  return n;
}

struct avl_node *avl_last(const struct avl_root *root) {
  struct avl_node *n;

  n = root->avl_node;
  if (!n)
    return NULL;
  while (n->avl_right)
    n = n->avl_right;
  return n;
}

struct avl_node *avl_next(const struct avl_node *node) {
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

struct avl_node *avl_prev(const struct avl_node *node) {
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
                      struct avl_root *root) {
  struct avl_node *parent = avl_parent(victim);
  __avl_change_child(victim, newnode, parent, root);
  if (victim->avl_left)
    avl_set_parent(victim->avl_left, newnode);
  if (victim->avl_right)
    avl_set_parent(victim->avl_right, newnode);

  *newnode = *victim;
}
