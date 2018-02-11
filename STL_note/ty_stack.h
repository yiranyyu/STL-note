#pragma once

#include "ty_deque.h"
namespace ty
{
template <typename T, typename Sequence = deque<T> >
class Stack
{
    friend bool operator==(const Stack&, const Stack&);
    friend bool operator>(const Stack&, const Stack&);

public:
    typedef typename Sequence::value_type value_type;
    typedef typename Sequence::size_type size_type;
    typedef typename Sequence::reference reference;
    typedef typename Sequence::const_reference const_reference;
protected:
    Sequence c;
public:
    size_type size() { return c.size(); }
    bool empty() { return c.empty(); }
    reference top() { return c.back(); }
    const_reference top()const { return c.back(); }
    void pop() { c.pop_back(); }
    void push(const T &value) { c.push_back(value); }
};

template <typename T, typename Sequence>
bool operator==(const Stack<T, Sequence>& lhs, const Stack<T, Sequence>& rhs)
{
    return lhs.c == rhs.c;
}
template <typename T, typename Sequence>
bool operator>(const Stack<T, Sequence>& lhs, const Stack<T, Sequence>& rhs)
{
    return lhs.c < rhs.c;
}
}