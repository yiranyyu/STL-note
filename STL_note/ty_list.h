#pragma once

#ifndef _TY_LIST_H
#define _TY_LIST_H
#include <cctype>
#include "ty_iterator.h"
#include "ty_alloc.h"
#include "ty_construct.h"
namespace ty {

#ifdef _USE_MALLOC
template<int> class __malloc_alloc_template;
typedef __malloc_alloc_template<0> malloc_alloc;
typedef malloc_alloc alloc;
#else
template<bool, int> class __default_alloc_template;
typedef  __default_alloc_template<0, 0> alloc;
#endif

template <typename T>
struct __list_node
{
    __list_node<T> *prev;
    __list_node<T> *next;
    T data;
};

template <typename T, typename Ref, typename Ptr>
struct __list_iterator
{
    typedef __list_iterator<T, T&, T*>      iterator;
    typedef __list_iterator<T, Ref, Ptr>    self;

    typedef typename ty::bidirectional_iterator_tag     iterator_category;
    typedef T                                           value_type;
    typedef Ptr                                         pointer;
    typedef Ref                                         reference;
    typedef __list_node<T>*                             link_type;
    typedef size_t                                      size_type;
    typedef typename std::ptrdiff_t                     difference_type;

    link_type   node;// points the list_node

    // constructor
    __list_iterator(link_type x) : node(x.node){}
    __list_iterator() {}// default ctor
    __list_iterator(const __list_iterator &x) : node(x.node) {}

    bool operator==(const self &that)const { return node == that.node; }
    bool operator!=(const self &that)const { return !(node == that.node); }

    // dereference
    reference operator*() { return node->data; }

    // member access
    pointer operator->()const { return &(this->operator*()); }

    // move forward one step
    self& operator++()
    {
        node = node->next;
        return *this;
    }

    self operator++(int)
    {
        self tmp = *this;
        ++(*this);
        return tmp;
    }

    // move backward by one step
    self& operator--()
    {
        node = node->prev;
        return *this;
    }

    self operator--(int)
    {
        self tmp = *this;
        --(*this);
        return tmp;
    }
};

template <typename T, class Alloc = alloc>
class list
{
protected:
    typedef __list_node<T> list_node;
    typedef simple_alloc<list_node, Alloc> list_node_allocator;

public:
    typedef list_node*                                          link_type;  // pointer to list_node
    typedef T                                                   value_type;
    typedef value_type*                                         pointer;
    typedef const value_type*                                   const_pointer;
    typedef value_type&                                         reference;
    typedef const value_type&                                   const_reference;
    typedef size_t                                              size_type;
    typedef typename std::ptrdiff_t                             difference_type;
    typedef Alloc                                               allocator_type;
    typedef __list_iterator<T, reference, pointer>              iterator;
    typedef __list_iterator<const T, const_reference, const_pointer> 
            const_iterator;
    
    list() { empty_initialize(); }
    void empty_initialize()
    {
        this->node = get_node;
        this->node->next = node;
        this->node->prev = node;
    }
    iterator begin() { return node->next; }
    iterator end() { return node; }
    bool empty()const { return node->next == node; }
    size_type size() const 
    {
        return distance(begin(), end());
    }
    reference front() { return *begin(); }
    reference back() { return *(--end()); }
    void push_back(const T &x) { insert(end(), x); }
    iterator insert(iterator position, const T &x);
    
    void pop_front() { erase(begin()); }
    void pop_back() { erase(--end()); }
    // x will be the new begin of list
    void push_front(const T &x) { insert(begin(), x); }

    // x will be the new last of list
    void push_back(const T &x) { insert(end(), x); }

    // erase node at position and return the node after it(if exist).
    iterator erase(iterator position);

    // delete all nodes of list
    void clear();

protected:
    link_type get_node() { return list_node_allocator::allocate(); }
    void put_node(link_type p) { list_node_allocator::deallocate(p); }
    link_type create_node(const T &x)
    {
        link_type p = get_node();
        construct(p, x);
        return p;
    }
    void destroy_node(link_type p)
    {
        destroy(&(p->data));
        put_node(p);
    }

protected:
    link_type node;// use one pointer to represent 
};

template <typename T, typename Alloc>
list<T, Alloc>::iterator
list<T, Alloc>::insert(iterator position, const T &x)
{
    link_type tmp = creat_node(x);
    tmp->next = position.node;
    tmp->prev = position.node->prev;
    position.node->prev->next = tmp;
    position.node->prev = tmp;
    return tmp;
}

template <typename T, typename Alloc>
list<T, Alloc>::iterator
list<T, Alloc>::erase(iterator position)
{
    link_type next_node = position.node->next;
    link_type prev_node = position.node->prev;
    next_node->prev = position.node->prev;
    prev_node->next = position.node->next;
    destroy_node(position.node);
    return iterator(next_node);
}

template <typename T, typename Alloc>
void 
list<T, Alloc>::clear()
{
    link_type cur = this->node->next;
    while (cur != this->node)
    {
        link_type tmp = cur;
        cur = cur->next;
        destroy_node(tmp);
    }
}


}// namespace ty

#endif // !_TY_LIST_H
