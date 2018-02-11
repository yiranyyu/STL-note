#pragma once
#include <iterator>
#include "ty_type_traits.h"
#include "ty_iterator.h"
#include "ty_alloc.h"
#include "ty_construct.h"
namespace ty
{
#ifdef _USE_MALLOC
template<int> class __malloc_alloc_template;
typedef __malloc_alloc_template<0> malloc_alloc;
typedef malloc_alloc alloc;
#else
template<bool, int> class __default_alloc_template;
typedef  __default_alloc_template<0, 0> alloc;
#endif

struct __slist_node_base
{
    __slist_node_base *next;
};

template <class T>
struct __slist_node : public __slist_node_base
{
    T data;
};

// insert a new node after prev_node 
inline
__slist_node_base* __slist_make_link(
    __slist_node_base *prev_node, 
    __slist_node_base *new_node)
{
    new_node->next = prev_node->next;
    prev_node->next = new_node;
    return new_node;
}

inline
size_t __slist_size(__slist_node_base *node)
{
    size_t result = 0;
    for (; node != nullptr; node = node->next) ++result;
    return result;
}

// slist 迭代器基本结构
struct __slist_iterator_base
{
    using size_type = size_t;
    using difference_type = ::std::ptrdiff_t;
    using iterator_category = forward_iterator_tag;

    __slist_node_base *node;
    
    // using node_base to construct
    __slist_iterator_base(__slist_node_base *x) :node(x) {}

    // increment
    void incr() { node = node->next; }

    bool operator==(const __slist_iterator_base &x)const { return node == x.node; }
    bool operator!=(const __slist_iterator_base &x)const { return node != x.node; }
};

// slist 迭代器结构
template <typename T, typename Ref, typename Ptr>
struct __slist_iterator : public __slist_iterator_base
{
    using iterator = __slist_iterator<T, T&, T*>;
    using const_iterator = __slist_iterator<T, const T&, const T*>;
    using self = __slist_iterator<T, Ref, Ptr>;

    using value_type = T;
    using pointer = Ptr;
    using reference = Ref;
    using list_node = __slist_node<T>;

    __slist_iterator(list_node *x) : __slist_iterator_base(x) {}
    __slist_iterator() : __slist_iterator_base(0) {}
    __slist_iterator(const iterator &x) : __slist_iterator_base(x.node) {};

    reference operator*()const { return ((list_node*)node)->data; }
    pointer operator->()const { return &(operator*();) }
    
    self &operator++() 
    {
        incr(); 
        return *this; 
    }
    self operator++(int) 
    {
        self tmp = *this;
        incr();
        return tmp; 
    }
};

template <typename T, typename Alloc = alloc>
class slist
{
public:
    using value_type        = T;
    using pointer           = value_type*;
    using const_pointer     = const value_type*;
    using reference         = value_type&;
    using const_reference   = const value_type&;
    using size_type         = size_t;
    using difference_type   = ::std::ptrdiff_t;

    using iterator = __slist_iterator<T, T&, T*>;
    using const_iterator = __slist_iterator<T, const T&, const T*>;

private:
    using list_node             = __slist_node<T>;
    using list_node_base        = __slist_node_base;
    using iterator_base         = __slist_iterator_base;
    using list_node_allocator   = simple_alloc<list_node, Alloc>;

    static list_node* create_node(const value_type &x)
    {
        list_node *node = list_node_allocator::allocate();
        try
        {
            construct(&node->data, x);
            node->next = nullptr;
        }
        catch (...) { list_node_allocator::deallocate(node); }
        return node;
    }

    static void destroy_node(list_node *node)
    {
        destroy(&node->data);
        list_node_allocator::deallocate(node);
    }
private:
    list_node_base head;// 注意， head 不是指针，是实物。

public:
    slist() { head.next = nullptr; }
    ~slist() { clear(); }
    iterator begin() { return iterator((list_node*)head.next;) }
    iterator end() { return iterator(0); }  // end return iterator of nullptr;
    size_type size()const { return __slist_size(head.next); }
    bool empty()const { return head.next == 0; }

    // just swap head of two slists
    void swap(slist &that)
    {
        list_node_base *tmp = head.next;
        head.next = that.head.next;
        that.head.next = tmp;
    }

public:
    reference front() { return ((list_node*)head.next)->data; }
    void push_front(const value_type &x)
    {
        __slist_make_link(&head, create_node(x));
    }

    void pop_front()
    {
        list_node *node = (list_node*)head.next;
        head.next = node->next;
        destroy_node(node);
    }
};
}