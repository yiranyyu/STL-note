#pragma once
#ifndef _TY_VECTOR_H
#define _TY_VECTOR_H

#include "ty_alloc.h"
#include "ty_uninitialized.h"
#include "ty_construct.h"
#include "ty_algobase.h"

namespace ty {

template <typename T, typename Alloc = alloc>
class vector
{
public:
    typedef T                   value_type;
    typedef value_type*         pointer;
    typedef const value_type*   const_pointer;
    typedef value_type*         iterator;
    typedef const value_type*   const_iterator;
    typedef value_type&         reference;
    typedef const value_type&   const_reference;
    typedef size_t              size_type;
    typedef ptrdiff_t      difference_type;
    typedef Alloc               allocator_type;

    vector() : start(0), finish(0), end_of_storage(0) {}
    vector(size_type n, const T &value) { fill_initialize(n, value); }
    // init with default value of value_type obj
    explicit vector(size_type n) { fill_initialize(n, value_type()); }

    ~vector()
    {
        destroy(start, finish);
        deallocate();
    }

    iterator begin() { return start; }
    iterator end() { return finish; }
    const_iterator begin()const { return static_cast<const_iterator>(start); }
    const_iterator end()const { return static_cast<const_iterator>(finish); }
    size_type size() const { return static_cast<size_type>(end() - begin()); }
    size_type capacity()const { return static_cast<size_type>(end_of_storage - begin()); }
    bool empty()const { return begin() == end(); }
    reference operator[](size_type index)
    {
        return const_cast<reference>(
            static_cast<const vector<T, Alloc>>(*this)[index]);
    }
    const_reference operator[](size_type index)const { return *(begin() + index); }

    reference front() { return *begin(); }
    reference back() { return *end(); }
    const reference front()const { return *begin(); }
    const reference back()const { return *end(); }

    void push_back(const T &value)
    {
        // if have remaining memory
        if (finish != end_of_storage)
        {
            construct(finish, value);
            ++finish;
        }
        else
            insert_aux(end(), value);
    }

    void pop_back()
    {
        if (empty())
            throw std::logic_error("pop from empty vector");
        // move finish first, since finish points the end of the last element in memory
        --finish;
        destroy(finish);
    }

    iterator erase(iterator position)
    {
        if (position + 1 != end())
            copy(position + 1, finish, position);
        --finish;
        destroy(finish);
        return position;
    }

    // erase elements of [start, end)
    iterator erase(iterator first, iterator last)
    {
        copy(last, end(), first);
        iterator new_finish = finish - (last - first);
        destroy(new_finish, finish);
        finish = new_finish;
    }

    void resize(size_type new_size, const T &value)
    {
        if (new_size < size())
            erase(begin() + new_size, end());
        else
            insert(end(), new_size - size(), value);
    }
    void resize(size_type new_size) { resize(new_size, T()); }
    void clear() { erase(begin(), end()); }
    void insert(iterator position, size_type n, const T &x);
protected:
    typedef simple_alloc<value_type, Alloc> data_allocator;
    iterator start;         // start of the space current used
    iterator finish;        // end of the space current used(i.e. points the end of the last elements(if exist))
    iterator end_of_storage;// end of the valid space

    // called by insert and push_back etc.
    void insert_aux(iterator position, const T& x);

    void deallocate()
    {
        // call deallocate with start adress and number of objs
        if (this->start)           
            data_allocator::deallocate(start, end_of_storage - start);
    }

    iterator allocate_and_fill(size_type n, const T& value)
    {
        iterator result = data_allocator::allocate(n);
        uninitialized_fill_n(result, n, value);
        return result;
    }

    // init and fill with copy of value
    void fill_initialize(size_type n, const T& value)
    {
        start = allocate_and_fill(n, value);
        finish = start + n;
        end_of_storage = finish;
    }

};

template<typename T, typename Alloc>
void
vector<T, Alloc>::insert_aux(iterator position, const T &x)
{
    if (finish != end_of_storage)
    {
        construct(finish, *(finish - 1)); // copy last element
        ++finish; // ����ˮλ
        copy_backward(position, finish - 2, finish - 1);
        *position = x;
    }
    else
    {
        const size_type old_size = size();
        const size_type len = old_size != 0 ? old_size * 2 : 1;
        iterator new_start = data_allocator::allocate(len); // alloc memory
        iterator new_finish = new_start;
        try
        {
            new_finish = uninitialized_copy(start, position, new_start);
            construct(new_finish, x);// init new element
            ++new_finish;
            new_finish = uninitialized_copy(position, finish, new_finish);
        }
        catch (...)
        {
            // "commit or rollback" semantics
            destroy(new_start, new_finish);
            data_allocator::deallocate(new_start, len);
            throw;
        }

        // destruct and deallocate old_vector
        destroy(begin(), end());
        deallocate();

        // adjust iterator to point the new vector
        start = new_start;
        finish = new_finish;
        end_of_storage = new_start + len;
    }
}

template <typename T, typename Alloc>
void
vector<T, Alloc>::insert(iterator position, size_type n, const T &x)
{
    // only process if n > 0
    if (0 == n)
        return;

    // the rest memory is enough for n elements
    if (static_cast<size_type>(end_of_storage - end()) >= n)
    {
        T x_copy = x;
        const size_type elems_after = finish - position;// number of elements after position(inclusive)
        iterator old_finish = finish;
        if (elems_after > n)
        {
            // number of elements after poistion more than n
            uninitialized_copy(finish - n, finish, finish);         // fill the extra space with elements to be moved
            finish += n;
            copy_backward(position, old_finish - n, old_finish);
            fill(position, position + n, x_copy);
        }
        else
        {
            uninitialized_fill_n(finish, n - elems_after, x_copy);  // fill the extra space with x_copy
            finish += n - elems_after;
            uninitialized_copy(position, old_finish, finish);
            finish += elems_after;
            fill(position, old_finish, x_copy);
        }
    }
    // rest space is not enough
    else
    {
        const size_type old_size = size();
        const size_type len = old_size + max(old_size, n);
        iterator new_start = data_allocator::allocate(len);
        iterator new_finish = new_start;
        try
        {
            // copy elements before position to new space
            new_finish = uninitialized_copy(start, position, new_start);
            // copy inserted elements to new space
            new_finish = uninitialized_fill_n(new_finish, n, x);
            // copy elements after position to new space
            new_finish = uninitialized_copy(position, finish, new_finish);
        }
        catch (...)
        {
            destroy(new_start, new_finish);
            data_allocator::deallocate(new_start, len);
            throw;
        }
        destroy(start, finish);
        deallocate();

        // move the memory tags
        start = new_start;
        finish = new_finish;
        end_of_storage = new_start + len;
    }
}

}// namespace ty

#endif // !_TY_VECTOR_H