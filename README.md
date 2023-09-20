# avltree
A fast, compact, non-recursive and instrusive avltree implementation.
Most part of this avltree is stolen from the brilliant [STL AVL map](https://sourceforge.net/projects/stlavlmap/).
In my test, when dealing with millions of nodes, deletion is about 10%~30% slower, insertion is about 10% slower than linux rbtree.
But when dealing with nearly ordered keys, insertion and deletion is much faster than linux rbtree.
