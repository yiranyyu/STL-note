#pragma once
#include "ty_iterator.h"
#include "ty_alloc.h"
#include "ty_construct.h"
#include <cstddef>

namespace ty
{
template <typename T1, typename T2>
struct pair
{
    using first_type = T1;
    using second_type = T2;
    first_type first;
    second_type second;
    pair() : first(T1()), second(T2()) {}
    pair(const T1 &a, const T2 &b) : first(a), second(b) {}
};

#ifdef _USE_MALLOC
template<int> class __malloc_alloc_template;
typedef __malloc_alloc_template<0> malloc_alloc;
typedef malloc_alloc alloc;
#else
template<bool, int> class __default_alloc_template;
typedef  __default_alloc_template<0, 0> alloc;
#endif

using __rb_tree_color_type = bool;
const __rb_tree_color_type __rb_tree_red = false;
const __rb_tree_color_type __rb_tree_black = true;


struct __rb_tree_node_base
{
    using color_type = __rb_tree_color_type;
    using base_ptr = __rb_tree_node_base*;  // ptr to tree node base

    color_type color;
    base_ptr parent;
    base_ptr left;
    base_ptr right;

    static base_ptr minimum(base_ptr x)
    {
        while (x->left) x = x->left;
        return x;
    }
    static base_ptr maximum(base_ptr x)
    {
        while (x->right) x = x->right;
        return x;
    }
};

template<typename Value>
struct __rb_tree_node : public __rb_tree_node_base
{
    using link_type = __rb_tree_node<Value>*;
    Value value_field;
};

struct __rb_tree_base_iterator
{
    using base_ptr          = __rb_tree_node_base::base_ptr;
    using iterator_category = bidirectional_iterator_tag;
    using difference_type   = ::std::ptrdiff_t;
    

    base_ptr node;// 用来与容器之间产生连结关系(make a reference)
    void increment()
    {
        if (node->right != nullptr)
        {
            // get the min of right sibling node 
            node = node->right;
            while (node->left) node = node->left;
        }
        else
        {
            base_ptr y = node->parent;
            while (node == y->right)// 一直上溯，直到 node 不是右结点
            {
                node = y;
                y = y->parent;
            }

            if (node->right != y) 
                node = y;
        }
    }
    void decrement()
    {
        // if node is header(i.e. node == end()), right child of head is the max of tree
        if (node->color == __rb_tree_red &&
            node->parent->parent == node)
            node = node->right;
        else if (node->left != nullptr)
        {
            // get the max in the left child tree
            node = node->left;
            while (node->right) node = node->right; 
        }
        else
        {
            base_ptr y = node->parent;
            while (node == y->left)
            {
                node = y;
                y = y->left;
            }
            node = y;
        }
    }
};

template<typename Value, typename Ref, typename Ptr>
struct __rb_tree_iterator : public __rb_tree_base_iterator
{
    using value_type        = Value;
    using reference         = Ref;
    using pointer           = Ptr;
    using iterator          = __rb_tree_iterator<Value, Value&, Value*>;
    using const_iterator    = __rb_tree_iterator<Value, const Value&, const Value*>;
    using self              = __rb_tree_iterator<Value, Ref, Ptr>;
    using link_type         = __rb_tree_node<Value>*;

    __rb_tree_iterator() {};
    __rb_tree_iterator(link_type x) { node = x; }
    __rb_tree_iterator(const iterator &it) { node = it.node; }

    reference operator*() const { return link_type(node)->value_field; }
    pointer operator->() const { return &(operator*()); }

    self& operator++() { increment(); return *this; }
    self operator++(int)
    {
        self tmp = *this;
        increment();
        return tmp;
    }

    self& operator--() { decrement(); return *this; }
    self  operator--(int)
    {
        self tmp = *this;
        decrement();
        return tmp;
    }

};



inline void
__rb_tree_rebalance(__rb_tree_node_base *x, __rb_tree_node_base* &root);
inline void
__rb_tree_rotate_right(__rb_tree_node_base *x, __rb_tree_node_base *&root);
inline void
__rb_tree_rotate_left(__rb_tree_node_base *x, __rb_tree_node_base *&root);


template <typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc = alloc>
class rb_tree
{
protected:
    using void_pointer = void*;
    using base_ptr = __rb_tree_node_base*;
    using rb_tree_node = __rb_tree_node<Value>;
    using rb_tree_node_allocator = simple_alloc<rb_tree_node, Alloc>;
    using color_type = __rb_tree_color_type;

public:
    using key_type = Key;
    using value_type = Value;
    using pointer = value_type*;
    using const_pointer = const value_type*;
    using reference = value_type&;
    using const_reference = const value_type&;
    using link_type = rb_tree_node*;
    using size_type = size_t;
    using difference_type = ::std::ptrdiff_t;
    using iterator = __rb_tree_iterator<Value, Value&, Value*>;
    using const_iterator = __rb_tree_iterator<Value, const Value&, const Value*>;

protected:
    link_type get_node() { return rb_tree_node_allocator::allocate(); }
    void put_node(link_type p) { rb_tree_node_allocator::deallocate(p); }

    link_type create_node(const value_type &x)
    {
        link_type tmp = get_node();
        try
        {
            construct(&tmp->value_field, x);
        }
        catch (...)
        {
            put_node(tmp);
        }
        return tmp;
    }

    link_type clone_node(link_type x)
    {
        link_type tmp = create_node(x->value_field);
        tmp->color = x->color;
        tmp->left = nullptr;
        tmp->right = nullptr;
        return tmp;
    }

    void destroy_node(link_type p)
    {
        destroy(&p->value_field);
        put_node(p);
    }

protected:
    // RB-tree 只以三笔数据表现
    size_type node_count;
    link_type header;
    Compare key_compare;

    // 以下三个函数用来方便取得header的成员
    link_type& root() const { return (link_type&)header->parent; }
    link_type& leftmost() const { return (link_type&)header->left; }
    link_type& rightmost() const { return (link_type&)header->right; }

    static link_type& left(link_type x)
    {
        return (link_type&)(x->left);
    }
    static link_type& right(link_type x)
    {
        return (link_type&)(x->right);
    }
    static link_type& parent(link_type x)
    {
        return (link_type&)(x->parent);
    }
    static reference value(link_type x)
    {
        return x->value_field;
    }
    static const Key& key(link_type x)
    {
        return KeyOfValue()(value(x));
    }
    static color_type& color(link_type x)
    {
        return (color_type&)(x->color);
    }

    static link_type& left(base_ptr x)
    {
        return (link_type&)(x->left);
    }
    static link_type& right(base_ptr x)
    {
        return (link_type&)(x->right);
    }
    static link_type& parent(base_ptr x)
    {
        return (link_type&)(x->parent);
    }
    static reference value(base_ptr x)
    {
        return ((link_type)x)->value_field;
    }
    static const Key& key(base_ptr x)
    {
        return KeyOfValue()(value(x));
    }
    static color_type& color(base_ptr x)
    {
        return (color_type&)(link_type(x)->color);
    }

    // get max and min value
    static link_type minimum(link_type x)
    {
        return (link_type)__rb_tree_node_base::minimum(x);
    }
    static link_type maximum(link_type x)
    {
        return (link_type)__rb_tree_node_base::maximum(x);
    }

public:
    rb_tree(const Compare& comp = Compare())
        :node_count(0), key_compare(comp) {
        init();
    }

    void clear() //<======================================================== bug
    {

    }

    ~rb_tree()
    {
        clear();
        put_node(header);
    }
    //rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& operator=(
    //    const rb_tree<Key, Value, KeyOfValue, Compare, Alloc> &that) 
    //{

    //};
public:
    // accessors
    Compare key_comp() const { return key_compare; }
    iterator begin() { return leftmost(); }
    iterator end() { return header; }
    bool empty() const { return node_count == 0; }
    size_type size() const { return node_count; }
    size_type max_size() const { return size_type(-1); }

    iterator find(const Key &key)
    {
        link_type y = header;   // last node which is not less than k
        link_type x = root();   // current node
        while (x != nullptr)
        {
            if (!key_compare(key(x), k))    y = x, x = left(x);
            else                            x = right(x);
        }
        iterator j = iterator(y);
        return (j == end() || key_compare(k, key(j.node))) ? end() : j;
    }

public:
    // 保持结点独一无二
    pair<iterator, bool> insert_unique(const value_type &x);
    // 允许节点值重复
    iterator insert_equal(const value_type &x);
private:
    iterator __insert(base_ptr x, base_ptr y, const value_type &v);
    link_type __copy(link_type x, link_type p);
    void __erase(link_type x);
    void init()
    {
        header = get_node();            // 产生一个结点空间，令header指向他
        color(header) = __rb_tree_red;  // set header color red, the distinguish it from the root, whitch is black.

        root() = 0;
        leftmost() = header;    // left child is self
        rightmost() = header;   // right child is self
    }
};

// 插入新值：节点键值允许重复
// 注意，返回值是一个RB-tree迭代器，指向新增节点
template <typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator
rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::insert_equal(const value_type &v)
{
    link_type y = header;
    link_type x = root();
    bool cmp = true;
    while (x != nullptr)
    {
        y = x;
        // 遇大则往左，否则往右
        cmp = key_compare(KeyOfValue()(v), key(x));
        x = cmp ? left(x) : right(x);
    }
    return __insert(x, y, v);
}
// 插入新值：节点键值不允许重复，若重复则插入无效
// 注意，返回值是个pair，第一个元素是个RB-tree迭代器，指向新增节点，
// 第二元素表示插入成功与否
template <typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
pair<typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator, bool>
rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::insert_unique(const value_type &v)
{
    link_type y = header;
    link_type x = root();
    bool cmp = true;    // init value is useful, Caution please
    while (x != nullptr)
    {
        y = x;
        cmp = key_compare(KeyOfValue()(v), key(x));
        x = cmp ? left(x) : right(x);
    }
    // now  y is the parent node of the postion 
    //      x is the pos to do insert
    iterator j = iterator(y);
    if (cmp)   // 如果离开循环时cmp为true, 表示遇到大，将要插入与左侧。
    {
        if (j == begin())   return pair<iterator, bool>(__insert(x, y, v), true);
        else                --j;// adjust j to test again
    }

    // if not equal to the parent
    if (key_compare(key(j.node), KeyOfValue()(v)))
        return pair<iterator, bool>(__insert(x, y, v), true);
    return pair<iterator, bool>(j, false);
}

template <typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator
rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::
__insert(base_ptr x_, base_ptr y_, const Value &v)
{
    link_type x = (link_type)x_;
    link_type y = (link_type)y_;
    link_type z;

    // key_compare 是键值大小比较准则，应该会是一个function object
    if (y == header || x != 0 || key_compare(KeyOfValue()(v), key(y)))
    {
        z = create_node(v); // make a new node
        left(y) = z;
        if (y == header)    // if tree is empty( ?????? I guess)
        {
            root() = z;
            rightmost() = z;
        }
        else if (y == leftmost())  // 保持header的left child 始终为leftmost
            leftmost = z;
    }
    else
    {
        z = create_node(v);
        right(y) = z;
        if (y == rightmost())   rightmost() = z; // keep the rightmost
    }

    parent(z)   = y;
    left(z)     = nullptr;
    right(z)    = nullptr;

    __rb_tree_rebalance(z, header->parent);// param 1 is the new node, param2 is the root of tree
    ++node_count;
    return iterator(z);
}

inline void
__rb_tree_rebalance(__rb_tree_node_base *x, __rb_tree_node_base* &root)
{
    x->color = __rb_tree_red;
    while (x != root && x->parent->color == __rb_tree_red)// parent node is red
    {
        // parent node is a left node of it's parent 
        if (x->parent == x->parent->parent->left)
        {
            // get the sibling node of x->parent
            __rb_tree_node_base *y = x->parent->parent->right;
            if (y != nullptr && y->color == __rb_tree_red)// sibling node of parent is not null and is red
            {
                x->parent->parent->color = __rb_tree_red;
                x->parent->color    = __rb_tree_black;
                y->color            = __rb_tree_black;
                x = x->parent->parent;
            }
            else  // x->parent have no sibling or only black sibling
            {
                if (x == x->parent->right)// if new node is right child of parent node
                {
                    x = x->parent;
                    __rb_tree_rotate_left(x, root);// first param is the point to rotate
                }
                x->parent->color = __rb_tree_black;
                x->parent->parent->color = __rb_tree_red;
                __rb_tree_rotate_right(x->parent->parent, root);
            }   
        }
        else // parent node is right child of it's parent
        {
            // get the sibling node of parent
            __rb_tree_node_base *y = x->parent->parent->left;
            if (y != nullptr && y->color == __rb_tree_red)
            {
                x->parent->color = __rb_tree_black;
                y->color         = __rb_tree_black;
                y->parent->parent->color = __rb_tree_red;
                x = x->parent->parent;
            }
            else
            {
                if (x == x->parent->left)   // if new node is a left node
                {
                    x = x->parent;
                    __rb_tree_rotate_right(x, root);// first param is the ratate point
                }
                x->parent->color = __rb_tree_black;
                x->parent->parent->color = __rb_tree_red;
                __rb_tree_rotate_left(x->parent->parent, root);
            }
        }
    }
    root->color = __rb_tree_black;
}

inline void
__rb_tree_rotate_left(__rb_tree_node_base *x, __rb_tree_node_base *&root)
{
    // x is the rotate point
    __rb_tree_node_base *y = x->right;
    x->right = y->left;
    if (y->left != nullptr)
    {
        y->left->parent = x;
    }
    y->parent = x->parent;

    // 令 y 完全顶替 x 的地位（必须将 x 对其父节点的关系完全接收过来）
    if      (x == root)             root == y;
    else if (x == x->parent->left)  x->parent->left = y;
    else                            x->parent->right = y;
    y->left   = x;
    x->parent = y;
}

inline void
__rb_tree_rotate_right(__rb_tree_node_base *x, __rb_tree_node_base *&root)
{
    __rb_tree_node_base *y = x->left;
    if (y->right != nullptr)    //别忘了设定父节点
        y->right->parent = x;
    y->parent = x->parent;

    if      (x == root)             root = y;
    else if (x == x->parent->right) x->parent->right = y;
    else                            x->parent->left = y;
    y->right = x;
    x->parent = y;
}
}