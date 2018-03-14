#pragma once
#include "ty_alloc.h"
#include <cctype>
#include <cstddef>
#include "ty_iterator.h"
#include "ty_uninitialized.h"
#include "ty_algobase.h"

namespace ty {

// Return number of element one buffer hold
//  i.e. if sz < 512, return 512 / sz
//       if sz >= 512, return 1
size_t __deque_buf_size(size_t sz)
{
    return (sz < 512 ? 512 / sz : 1);
}

// BufSiz : number of elements one buffer hold
template <typename T, typename Ref, typename Ptr>
struct __deque_iterator
{
    typedef __deque_iterator<T, T&, T*>                 iterator;
    typedef __deque_iterator<T, const T&, const T*>     const_iterator;

    static size_t buffer_size() { return __deque_buf_size(sizeof(T)); }
    
    using iterator_category = random_access_iterator_tag;
    using value_type = T;
    using reference = Ref;
    using pointer = Ptr;
    using size_type = size_t;
    using difference_type = ptrdiff_t;
    using map_pointer = T * *; // pointer to map of deque
    using self = __deque_iterator<T, Ref, Ptr>;

    // Data member
    T *cur;
    T *first;
    T *last;
    map_pointer node;

    __deque_iterator(void *)
        :cur(0), first(0), last(0), node(0)
    {}

    // set new node 
    void set_node(map_pointer new_node)
    {
        node = new_node;
        first = *new_node;
        last = first + difference_type(buffer_size());
    }
    
    reference operator*()const { return *cur; }
    reference operator->()const { return cur; }
    difference_type operator-(const self &x) const
    {
        return difference_type(buffer_size()) * (node - x.node - 1) +
            (cur - first) + (x.last - x.cur);
    }

    self& operator++()
    {
        ++cur;
        if (cur == last)
        {
            set_node(node + 1);
            cur = first;
        }
        return *this;
    }

    self operator++(int)
    {
        self tmp = *this;
        ++(*this);
        return tmp;
    }

    self& operator--()
    {
        if (cur == first)
        {
            set_node(node - 1);
            cur = last;
        }
        --cur;
        return *this;
    }

    self operator--(int)
    {
        self tmp = *this;
        --*this;
        return tmp;
    }

    self& operator+=(difference_type n)
    {
        difference_type offset = n + (cur - first);
        if (offset >= 0 && offset < difference_type(buffer_size()))
        {
            // dst is in same buffer
            cur += n;
        }
        else
        {
            // dst is in another buffer
            difference_type node_offset =
                offset > 0 ? offset / difference_type(buffer_size())
                : -difference_type((-offset - 1) / buffer_size()) - 1;
            // shift to right node
            set_node(node + node_offset);

            // shift to right element
            cur = first + (offset - node_offset * difference_type(buffer_size()));
        }
        return *this;
    }

    self operator+(difference_type n)const
    {
        return self(*this) += n;
    }

    self& operator-=(difference_type n)
    {
        return (*this) += -n;
    }

    self operator-(difference_type n)const
    {
        return self(*this) -= n;
    }
    
    // do random access
    reference operator[](difference_type n)
    {
        return *(*this + n);
    }

    bool operator==(const self &that)const { return this->cur == that.cur; }
    bool operator!=(const self &that)const { return !(*this == that); }
    bool operator<(const self &that)const
    {
        return (node == that.node ? (cur < that.cur) : (node < that.node));
    }
};

template <typename T, typename Alloc = alloc>
class deque
{
public:
    typedef T                                       value_type;
    typedef size_t                                  size_type;
    typedef value_type*                             pointer;
    typedef const value_type*                       const_pointer;
    typedef value_type&                             reference;
    typedef const value_type&                       const_reference;
    typedef typename std::ptrdiff_t                 difference_type;
    typedef Alloc                                   allocator_type;
    typedef __deque_iterator<T, T&, T*>             iterator;
    typedef __deque_iterator<T, const T&, const T*> const_iterator;

protected:
    typedef pointer*                            map_pointer;// pointer of map
    typedef simple_alloc<pointer, Alloc>        map_allocator;
    typedef simple_alloc<value_type, Alloc>     data_allocator;

protected:  // Data members
    map_pointer map;
    size_type map_size;
    iterator start;     // point at first element in deque
    iterator finish;    // point after last element in deque

public:
    deque(size_type n, const value_type &value)
        :start(0), finish(0), map(0), map_size(0)
    { 
        fill_initialize(n, value);
    }

    iterator begin() { return start; }
    iterator end() { return finish; }

    reference operator[](size_type n)
    {
        return start[difference_type(n)];
    }
    
    reference front() { return *start; }
    const_reference front()const { return *start; }
    reference back()
    {
        iterator tmp = finish;
        --tmp;
        return *tmp;
    }

    const_reference back()const
    {
        iterator tmp = finish;
        --tmp;
        return *tmp;
    }

    // call iterator::operator-(const iterator&)
    size_type size()const { return static_cast<size_type>(finish - start); }

    size_type max_size()const { return size_type(-1); }

    bool empty()const { return finish == start; }

    void push_back(const value_type &value)
    {
        if (finish.cur != finish.last - 1)
        {
            // if have two or more element size space
            construct(finish.cur, value);
            ++finish.cur;
        }
        else
            push_back_aux(value);
    }
    void push_back_aux(const value_type &value);

    void push_front(const value_type &value)
    {
        if (start.cur != start.first)
        {
            construct(start.cur - 1, value);
            --start.cur;
        }
        else
            push_front_aux(value);
    }
    void push_front_aux(const value_type &value);
    
    void reserve_map_at_back(size_type nodes_to_add = 1)
    {
        if (nodes_to_add + 1 > map_size - (finish.node - map))
            reallocate_map(nodes_to_add, false);
    }

    void reserve_map_at_front(size_type nodes_to_add = 1)
    {
        if (nodes_to_add > start.node - map)
            reallocate_map(nodes_to_add, true);
    }
    void reallocate_map(size_type nodes_to_add, bool add_at_front);

    void pop_back()
    {
        if (finish.cur != finish.first)
        {
            --finish.cur;
            destroy(finish.cur);
        }
        else
            pop_back_aux(); // free memory
    }
    void pop_back_aux();

    void pop_front()
    {
        if (start.cur != start.last - 1)
        {
            destroy(start.cur);
            ++start.cur;
        }
        else
            pop_front_aux();
    }
    void pop_front_aux();

    void clear();
    iterator erase(iterator pos);

    // erase element in range [first, last)
    iterator erase(iterator first, iterator last);

    iterator insert(iterator position, const value_type &value);
    iterator insert_aux(iterator pos, const value_type &x);
private:
    static size_type initial_map_size() { return 8; }
    void create_map_and_nodes(size_type num_elements);
    void fill_initialize(size_type n, const value_type &value);
    pointer allocate_node()
    {
        auto p = data_allocator::allocate(iterator::buffer_size());
        return p;
    }
    void deallocate_node(pointer p)
    {
        data_allocator::deallocate(p, iterator::buffer_size());
    }
};

template <typename T, typename Alloc>
void deque<T, Alloc>::fill_initialize(size_type n, const value_type &value)
{
    create_map_and_nodes(n);
    map_pointer cur;
    try
    {
        for (cur = start.node; cur < finish.node; ++cur)
            uninitialized_fill(*cur, *cur + iterator::buffer_size(), value);
        uninitialized_fill(finish.first, finish.cur, value);
    }
    catch (...)
    {
        if (cur == finish.node)
            destroy(finish.first, finish.cur);

        for (; cur >= finish.node; --cur)
            destroy(*cur, *cur + iterator::buffer_size());
    }
}

// num_elements : number of elements to init
template <typename T, typename Alloc>
void deque<T, Alloc>::create_map_and_nodes(size_type num_elements)
{
    // 所需节点数 = （元素个数 / 每个缓冲区可容纳的元素个数） + 1
    // 如果刚好整除，则多分配一个节点
    size_type num_nodes = num_elements / iterator::buffer_size() + 1;
    
    // a map manage some nodes, at least 8, at most : nodes needed plus 2
    map_size = max(initial_map_size(), num_nodes + 2);
    map = map_allocator::allocate(map_size);

    // 以下令 nstart 和 nfinish指 map所拥有之全部节点的最中央区域
    // 保持在最中央，可使头尾两端的扩充能量一样大。每个节点对应一个缓冲区
    map_pointer nstart = map + (map_size - num_nodes) / 2;
    map_pointer nfinish = nstart + num_nodes - 1;

    map_pointer cur;
    try
    {
        //为每个现用节点配置缓冲区
        for (cur = nstart; cur <= nfinish; ++cur)
            *cur = allocate_node();
    }
    catch (...)
    {
        destroy(nstart, cur);
        throw;
    }
    start.set_node(nstart);
    finish.set_node(nfinish);
    start.cur = start.first;
    finish.cur = finish.first + num_elements % iterator::buffer_size();

}

template <typename T, typename Alloc>
void deque<T, Alloc>::push_back_aux(const value_type &value)
{
    reserve_map_at_back();  // 若符合某种条件则重换一个map
    *(finish.node + 1) = allocate_node();
    try
    {
        construct(finish.cur, value);
        finish.set_node(finish.node + 1);
        finish.cur = finish.first;
    }
    catch (...)
    {
        deallocate_node(*(finish.node + 1));
        throw;
    }
}

template <typename T, typename Alloc>
void deque<T, Alloc>::push_front_aux(const value_type &value)
{
    reserve_map_at_front();
    *(start.node - 1) = allocate_node();
    try
    {
        start.set_node(start.node - 1);
        start.cur = start.last - 1;
        construct(start.cur, value);
    }
    catch (...)
    {
        start.set_node(start.node + 1);
        start.cur = start.first;
        deallocate_node(*(start.node - 1));
        throw;
    }
}

template <typename T, typename Alloc>
void deque<T, Alloc>::reallocate_map(size_type nodes_to_add, bool add_at_front)
{
    size_type old_num_nodes = finish.node - start.node + 1;
    size_type new_num_nodes = old_num_nodes + nodes_to_add;
    map_pointer new_nstart;
    if (map_size > 2 * new_num_nodes)
    {
        new_nstart = map + (map_size - new_num_nodes) / 2
            + (add_at_front ? nodes_to_add : 0);
        if (new_nstart < start.node)
            copy(start.node, finish.node + 1, new_nstart);
        else
            copy_backward(start.node, finish.node + 1, new_nstart + old_num_nodes);
    }
    else
    {
        size_type new_map_size = map_size + max(map_size, nodes_to_add) + 2;
        map_pointer new_map = map_allocator::allocate(new_map_size);
        new_nstart = new_map + ((new_map_size - new_num_nodes) / 2
            + (add_at_front ? nodes_to_add : 0));
        copy(start.node, finish.node + 1, new_nstart);
        map_allocator::deallocate(map, map_size);
        
        map = new_map;
        map_size = new_map_size;
    }
    
    start.set_node(new_nstart);
    finish.set_node(new_nstart + old_num_nodes - 1);
}

template <typename T, typename Alloc>
void deque<T, Alloc>::pop_back_aux()
{
    deallocate_node(finish.first);  // free the buffer start at finish.first
    finish.set_node(finish.node - 1);
    finish.cur = finish.last - 1;
    destroy(finish.cur);    // finish points the end of last element
}

template <typename T, typename Alloc>
void deque<T, Alloc>::pop_front_aux()
{
    destroy(start.cur);

    deallocate_node(start.first);
    start.set_node(start.node + 1);
    start.cur = start.first;
}

template <typename T, typename Alloc>
void deque<T, Alloc>::clear()
{
    // for every buffer except for start.node and finish.node
    for (map_pointer node = start.node + 1; node < finish.node; ++node)
    {
        destroy(*node, *node + iterator::buffer_size());
        data_allocator::deallocate(*node, iterator::buffer_size());
    }

    // if have at least two node buffer
    if (start.node != finish.node)
    {
        destroy(start.cur, start.last);     // elements in first buffer
        destroy(finish.first, finish.cur);  // elements in last buffer
        // 此处保留头缓冲区
        data_allocator::deallocate(finish.first, iterator::buffer_size());
    }
    else
        destroy(start.cur, finish.cur);     // if only one buffer

    finish = start; // adjust to default. i.e. one buffer in memory
}

template <typename T, typename Alloc>
typename deque<T, Alloc>::iterator 
deque<T, Alloc>::erase(iterator pos)
{
    iterator next = ++iterator(pos);
    difference_type index = pos - start;// call __deque_iterator::operator-(const iterator&)
    
    if (index < (size() >> 1))  // if elements before index is less than after
    {
        copy_backward(start, pos, next);
        pop_front();
    }
    else
    {
        copy(next, finish, pos);
        pop_back();
    }
    return start + index;

}

template <typename T, typename Alloc>
typename deque<T, Alloc>::iterator
deque<T, Alloc>::erase(iterator first, iterator last)
{
    if (first == start && last == finish)
    {
        clear();
        return start;   // start == finish after clear 
    }
    else
    {
        difference_type n = last - first;
        difference_type elems_before = first - start;
        if (elems_before < (size() - n) / 2)    // if elements before first more than after
        {
            copy_backward(start, first, last);
            iterator new_start = start + n;
            destroy(start, new_start);  // destroy rest elements

            // free rest buffers
            for (map_pointer cur = start.node; cur < new_start.node; ++cur)
                data_allocator::deallocate(*cur, iterator::buffer_size());
            start = new_start;
        }
        else                                    // if elements before first not more than after
        {
            copy(last, finish, first);
            iterator new_finish = finish - n;
            destroy(new_finish, finish);

            for (map_pointer cur = finish.node; cur <= new_finish.node; --cur)
                data_allocator::deallocate(*cur, iterator::buffer_size());
        }
        return start + elems_before;
    }
}

template <typename T, typename Alloc>
typename deque<T, Alloc>::iterator
deque<T, Alloc>::insert(iterator position, const value_type &value)
{
    if (position.cur == start.cur)
    {
        push_front(value);
        return start;
    }
    else if (position.cur == finish.cur)
    {
        push_back(value);
        return --end();
    }
    else
    {
        return insert_aux(position, value);
    }
}

template <typename T, typename Alloc>
typename deque<T, Alloc>::iterator
deque<T, Alloc>::insert_aux(iterator pos, const value_type &x)
{
    difference_type index = pos - start;
    value_type x_copy = x;
    if (index < size() / 2)
    {
        push_front(front());    // add one TAG to help move
        iterator front1 = start;
        ++front1;
        iterator front2 = front1;
        ++front2;
        pos = start + index;
        iterator pos1 = pos;
        ++pos1;
        copy(front2, pos1, front1);
    }
    else
    {
        push_back(back());
        iterator back1 = finish;
        --back1;
        iterator back2 = back1;
        --back2;
        pos = start + index;
        copy_backward(pos, back2, back1);
    }
    *pos = x_copy;
    return pos;
}

}
