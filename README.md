# avltree
A fast, compact, non-recursive and instrusive avltree implementation.
Most part of this avltree is stolen from [STL AVL map](https://sourceforge.net/projects/stlavlmap/).
In my test, when dealing with millions of nodes, deletion is about 10%~30% slower, insertion is about 10% faster than linux rbtree.
But when dealing with nearly ordered keys, insertion and deletion of avltree is much faster than linux rbtree.
