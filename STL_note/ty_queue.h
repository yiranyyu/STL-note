#pragma once

#include "ty_deque.h"
namespace ty
{
template <typename T, typename Sequence = deque<T> >
class Queue
{
    friend bool operator==(const Queue&, const Queue&);
    friend bool operator<(const Queue&, const Queue&);
public:
    typedef typename Sequence::value_type value_type;
    typedef typename Sequence::size_type size_type;
    typedef typename Sequence::reference reference;
    typedef typename Sequence::const_reference const_reference;
protected:
    Sequence c;
public:
    size_type size() { return c.size(); }
    bool empty() { return c.empty; }
    reference front() { return c.front(); }
    const_reference front() const{ return c.front(); }
    reference back() { return c.back(); }
    const_reference back() const{ return c.back(); }
    void push(const T& value) { return c.push_back(value); }
    void pop() { return c.pop_front(); }
};

template <typename T, typename Sequence>
bool operator==(const Queue<T, Sequence>& lhs, const Queue<T, Sequence>& rhs)
{
    return lhs.c == rhs.c;
}
template <typename T, typename Sequence>
bool operator<(const Queue<T, Sequence>& lhs, const Queue<T, Sequence>& rhs)
{
    return lhs.c < rhs.c;
}

}