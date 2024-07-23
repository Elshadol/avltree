#include "avltree.h"

static inline void avl_right2balance(struct avl_node *n) {
  n->__avl_parent_balance -= AVL_RIGHT_HEAVY;
}

static inline void avl_left2balance(struct avl_node *n) {
  n->__avl_parent_balance -= AVL_LEFT_HEAVY;
}

static inline void avl_balance2right(struct avl_node *n) {
  n->__avl_parent_balance += AVL_RIGHT_HEAVY;
}

static inline void avl_balance2left(struct avl_node *n) {
  n->__avl_parent_balance += AVL_LEFT_HEAVY;
}

static inline struct avl_node *avl_parent(const struct avl_node *n) {
  return (struct avl_node *)(n->__avl_parent_balance & ~3lu);
}

static inline struct avl_node *avl_bal_parent(const struct avl_node *n) {
  return (struct avl_node *)n->__avl_parent_balance;
}

static inline unsigned long avl_balance(const struct avl_node *n) {
  return (unsigned long)(n->__avl_parent_balance & 3lu);
}

static inline void avl_set_parent(struct avl_node *n, struct avl_node *p) {
  n->__avl_parent_balance = avl_balance(n) + (unsigned long)(p);
}

static inline void avl_set_parent_balance(struct avl_node *n,
                                          struct avl_node *p,
                                          unsigned long balance) {
  n->__avl_parent_balance = (unsigned long)p + balance;
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
  avl_set_parent_balance(old, new, oldb);
  avl_set_parent_balance(new, parent, newb);
  __avl_change_child(old, new, parent, root);
}

void avl_insert_rebalance(struct avl_node *node, struct avl_root *root) {
  unsigned long balance1, balance2, balance3;
  struct avl_node *parent = avl_bal_parent(node), *tmp1, *tmp2;
  while (parent) {
    balance1 = avl_balance(parent);
    if (node == parent->avl_left) {
      if (balance1 == AVL_RIGHT_HEAVY) {
        avl_right2balance(parent);
        break;
      } else if (balance1 == AVL_BALANCED) {
        node = parent;
        parent = avl_bal_parent(node);
        avl_balance2left(node);
        continue;
      } else {
        balance1 = AVL_BALANCED;
        balance2 = avl_balance(node);
        if (balance2 == AVL_RIGHT_HEAVY) {
          balance2 = AVL_BALANCED;
          tmp1 = node->avl_right;
          balance3 = avl_balance(tmp1);
          if (balance3 == AVL_LEFT_HEAVY)
            balance1 = AVL_RIGHT_HEAVY;
          else if (balance3 == AVL_RIGHT_HEAVY)
            balance2 = AVL_LEFT_HEAVY;
          node->avl_right = tmp2 = tmp1->avl_left;
          tmp1->avl_left = node;
          if (tmp2)
            avl_set_parent(tmp2, node);
          avl_set_parent_balance(node, tmp1, balance2);
          node = tmp1;
        }
        balance3 = AVL_BALANCED;
        parent->avl_left = tmp1 = node->avl_right;
        node->avl_right = parent;
        if (tmp1)
          avl_set_parent(tmp1, parent);
        __avl_rotate_set_parent(parent, node, balance1, balance3, root);
        break;
      }
    } else {
      if (balance1 == AVL_LEFT_HEAVY) {
        avl_left2balance(parent);
        break;
      } else if (balance1 == AVL_BALANCED) {
        node = parent;
        parent = avl_bal_parent(node);
        avl_balance2right(node);
        continue;
      } else {
        balance1 = AVL_BALANCED;
        balance2 = avl_balance(node);
        if (balance2 == AVL_LEFT_HEAVY) {
          balance2 = AVL_BALANCED;
          tmp1 = node->avl_left;
          balance3 = avl_balance(tmp1);
          if (balance3 == AVL_LEFT_HEAVY)
            balance2 = AVL_RIGHT_HEAVY;
          else if (balance3 == AVL_RIGHT_HEAVY)
            balance1 = AVL_LEFT_HEAVY;
          node->avl_left = tmp2 = tmp1->avl_right;
          tmp1->avl_right = node;
          if (tmp2)
            avl_set_parent(tmp2, node);
          avl_set_parent_balance(node, tmp1, balance2);
          node = tmp1;
        }
        balance3 = AVL_BALANCED;
        parent->avl_right = tmp1 = node->avl_left;
        node->avl_left = parent;
        if (tmp1)
          avl_set_parent(tmp1, parent);
        __avl_rotate_set_parent(parent, node, balance1, balance3, root);
        break;
      }
    }
  }
}

static void __avl_erase_rebalance(struct avl_node *node,
                                  struct avl_node *parent,
                                  struct avl_root *root) {
  unsigned long balance1, balance2, balance3;
  struct avl_node *sibling, *tmp1, *tmp2;

  if (parent->avl_left == parent->avl_right) {
    node = parent;
    parent = avl_parent(node);
    node->__avl_parent_balance = (unsigned long)parent;
    if (!parent)
      return;
  }

  do {
    balance1 = avl_balance(parent);
    sibling = parent->avl_right;
    if (node != sibling) {
      if (balance1 == AVL_BALANCED) {
        avl_balance2right(parent);
        break;
      } else if (balance1 == AVL_LEFT_HEAVY) {
        node = parent;
        avl_left2balance(node);
        parent = avl_bal_parent(node);
        continue;
      } else {
        balance1 = AVL_BALANCED;
        balance3 = AVL_BALANCED;
        balance2 = avl_balance(sibling);
        if (balance2 == AVL_LEFT_HEAVY) {
          balance2 = AVL_BALANCED;
          tmp1 = sibling->avl_left;
          balance3 = avl_balance(tmp1);
          if (balance3 == AVL_RIGHT_HEAVY)
            balance1 = AVL_LEFT_HEAVY;
          else if (balance3 == AVL_LEFT_HEAVY)
            balance2 = AVL_RIGHT_HEAVY;
          sibling->avl_left = tmp2 = tmp1->avl_right;
          tmp1->avl_right = sibling;
          if (tmp2)
            avl_set_parent(tmp2, sibling);
          avl_set_parent_balance(sibling, tmp1, balance2);
          sibling = tmp1;
          balance3 = AVL_BALANCED;
        } else if (balance2 == AVL_BALANCED) {
          balance1 = AVL_RIGHT_HEAVY;
          balance3 = AVL_LEFT_HEAVY;
        }
        parent->avl_right = tmp1 = sibling->avl_left;
        sibling->avl_left = parent;
        if (tmp1)
          avl_set_parent(tmp1, parent);
        __avl_rotate_set_parent(parent, sibling, balance1, balance3, root);
        if (balance1 == AVL_RIGHT_HEAVY)
          break;
        node = sibling;
        parent = avl_bal_parent(node);
        continue;
      }
    } else {
      sibling = parent->avl_left;
      if (balance1 == AVL_BALANCED) {
        avl_balance2left(parent);
        break;
      } else if (balance1 == AVL_RIGHT_HEAVY) {
        node = parent;
        avl_right2balance(node);
        parent = avl_bal_parent(node);
        continue;
      } else {
        balance1 = AVL_BALANCED;
        balance3 = AVL_BALANCED;
        balance2 = avl_balance(sibling);
        if (balance2 == AVL_RIGHT_HEAVY) {
          balance2 = AVL_BALANCED;
          tmp1 = sibling->avl_right;
          balance3 = avl_balance(tmp1);
          if (balance3 == AVL_LEFT_HEAVY)
            balance1 = AVL_RIGHT_HEAVY;
          else if (balance3 == AVL_RIGHT_HEAVY)
            balance2 = AVL_LEFT_HEAVY;
          sibling->avl_right = tmp2 = tmp1->avl_left;
          tmp1->avl_left = sibling;
          if (tmp2)
            avl_set_parent(tmp2, sibling);
          avl_set_parent_balance(sibling, tmp1, balance2);
          sibling = tmp1;
          balance3 = AVL_BALANCED;
        } else if (balance2 == AVL_BALANCED) {
          balance1 = AVL_LEFT_HEAVY;
          balance3 = AVL_RIGHT_HEAVY;
        }
        parent->avl_left = tmp1 = sibling->avl_right;
        sibling->avl_right = parent;
        if (tmp1)
          avl_set_parent(tmp1, parent);
        __avl_rotate_set_parent(parent, sibling, balance1, balance3, root);
        if (balance1 == AVL_LEFT_HEAVY)
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
    tmp->__avl_parent_balance = node->__avl_parent_balance;
  }
  __avl_change_child(node, tmp, parent1, root);
  if (child)
    child->__avl_parent_balance = (unsigned long)parent;
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
