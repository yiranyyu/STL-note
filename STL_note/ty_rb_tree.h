#pragma once
#include "ty_iterator.h"
#include "ty_alloc.h"
#include "ty_construct.h"

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
        while (x->left) 
            x = x->left;
        return x;
    }
    static base_ptr maximum(base_ptr x)
    {
        while (x->right) 
            x = x->right;
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
    using difference_type   = ptrdiff_t;
    

    base_ptr node;// ����������֮����������ϵ(make a reference)
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
            while (node == y->right)// һֱ���ݣ�ֱ�� node �����ҽ��
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

    bool operator==(const self &that)const noexcept
    {
        return this->node == that.node;
    }

    bool operator!=(const self &that)const noexcept
    {
        return this->node != that.node;
    }

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
    // RB-tree ֻ���������ݱ���
    size_type node_count;
    link_type header;
    Compare key_compare;

    // ��������������������ȡ��header�ĳ�Ա
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
        return (color_type&)(x->color);
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
        // TODO
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
    const_iterator begin() const{ return leftmost(); }
    const_iterator end() const { return header; }
    bool empty() const { return node_count == 0; }
    size_type size() const { return node_count; }
    size_type max_size() const { return size_type(-1); }

    iterator find(const Key &key)
    {
        link_type y = header;   // last node which is not less than k
        link_type x = root();   // current node
        while (x != nullptr)
        {
            if (!key_compare(Key(x), key))    y = x, x = left(x);
            else                            x = right(x);
        }
        iterator j = iterator(y);
        return (j == end() || key_compare(key, Key(j.node))) ? end() : j;
    }

public:
    // ���ֽ���һ�޶�
    pair<iterator, bool> insert_unique(const value_type &x);
    // ����ڵ�ֵ�ظ�
    iterator insert_equal(const value_type &x);
private:
    iterator __insert(base_ptr x, base_ptr y, const value_type &v);
    link_type __copy(link_type x, link_type p);
    void __erase(link_type x);
    void init()
    {
        header = get_node();            // ����һ�����ռ䣬��headerָ����
        color(header) = __rb_tree_red;  // set header color red, the distinguish it from the root, whitch is black.

        root() = 0;
        leftmost() = header;    // left child is self
        rightmost() = header;   // right child is self
    }
};

// ������ֵ���ڵ��ֵ�����ظ�
// ע�⣬����ֵ��һ��RB-tree��������ָ�������ڵ�
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
        // ���������󣬷�������
        cmp = key_compare(KeyOfValue()(v), key(x));
        x = cmp ? left(x) : right(x);
    }
    return __insert(x, y, v);
}
// ������ֵ���ڵ��ֵ�������ظ������ظ��������Ч
// ע�⣬����ֵ�Ǹ�pair����һ��Ԫ���Ǹ�RB-tree��������ָ�������ڵ㣬
// �ڶ�Ԫ�ر�ʾ����ɹ����
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
    if (cmp)   // ����뿪ѭ��ʱcmpΪtrue, ��ʾ�����󣬽�Ҫ��������ࡣ
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

    // key_compare �Ǽ�ֵ��С�Ƚ�׼��Ӧ�û���һ��function object
    if (y == header || x != 0 || key_compare(KeyOfValue()(v), key(y)))
    {
        z = create_node(v); // make a new node
        left(y) = z;
        if (y == header)    // if tree is empty( ?????? I guess)
        {
            root() = z;
            rightmost() = z;
        }
        else if (y == leftmost())  // ����header��left child ʼ��Ϊleftmost
            leftmost() = z;
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

    // �� y ��ȫ���� x �ĵ�λ�����뽫 x ���丸�ڵ�Ĺ�ϵ��ȫ���չ�����
    if      (x == root)             root = y;
    else if (x == x->parent->left)  x->parent->left = y;
    else                            x->parent->right = y;
    y->left   = x;
    x->parent = y;
}

inline void
__rb_tree_rotate_right(__rb_tree_node_base *x, __rb_tree_node_base *&root)
{
    __rb_tree_node_base *y = x->left;
    if (y->right != nullptr)    //�������趨���ڵ�
        y->right->parent = x;
    y->parent = x->parent;

    if      (x == root)             root = y;
    else if (x == x->parent->right) x->parent->right = y;
    else                            x->parent->left = y;
    y->right = x;
    x->parent = y;
}
}