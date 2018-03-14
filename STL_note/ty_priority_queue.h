#pragma once
#include "ty_heap.h"
#include "ty_vector.h"
namespace ty
{
template <typename T, typename Sequence = vector<T>,
    typename Compare = less<typename Sequence::value_type>>
    class priority_queue
{
public:
    using value_type = Sequence::value_type;
    using size_type = Sequence::size_type;
    using reference = Sequence::reference;
    using const_reference = Sequence::const_reference;
protected:
    Sequence c; // container
    Compare  cmp;
public:
    priority_queue() :c() {}
    explicit priority_queue(const Compare &x) : c(), cmp(x) {}

    template <typename InputIterator>
    priority_queue(InputIterator first, InputIterator last, const Compare &x)
        : c(first, last), cmp(x)
    {
        make_heap(c.begin(), c.end(), cmp);
    }

    template <typename InputIterator>
    priority_queue(InputIterator first, InputIterator last)
        : c(first, last)
    {
        make_heap(c.begin(), c.end(), cmp);
    }

    bool empty()const { return c.empty(); }
    size_type size()const { return c.size(); }
    const_reference top()const { return c.front(); }
    void push(const value_type &x)
    {
        try
        {
            c.push_back(x);
            push_heap(c.begin(), c.end(), cmp);
        }
        catch (...) { c.clear(); }
    }

    void pop()
    {
        try
        {
            pop_heap(c.begin(), c.end(), cmp);
            c.pop_back();
        }
        catch (...) { c.clear(); }
    }
};
}