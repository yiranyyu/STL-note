#pragma once
#include "ty_alloc.h"
#include "ty_rb_tree.h"
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

template<typename Key, typename Value, typename Compare = less<Key>, typename Alloc = alloc>
class map
{
public:
    using key_type      = Key;
    using data_type     = Value;
    using mapped_type   = Value;
    using value_type    = pair<const Key, Value>;  // 键值类型为const Key以禁止修改键值
    using key_compare   = Compare;

    // 定义一个functor，其作用就是调用"元素比较函数"
    class value_compare
    {
        friend class map<Key, Value, Compare, Alloc>;
    protected:
        Compare comp;
        value_compare(Compare c) : comp(c) {}
    public:
        bool operator()(const value_type &x, const value_type &y) const {
            return comp(x.first, y.first);// compare the key of value pair
        }
    };
    template<typename pair_type>
    struct select1st
    {
        typename pair_type::first operator()(const pair_type &x) { return x.first; }
    };

private:
    using rep_type = rb_tree<key_type, value_type, select1st<value_type>, key_compare, Alloc>;
    rep_type t; // using rb_tree to represent the set

public:
    using pointer = rep_type::pointer;
    using const_pointer = rep_type::const_pointer;
    using reference = rep_type::reference;
    using const_reference = rep_type::const_reference;
    using iterator = rep_type::iterator;
    using const_iterator = rep_type::const_iterator;
    using size_type = rep_type::size_type;
    using difference_type = rep_type::difference_type;
    // ... other functions and types 
    pair<iterator, bool> insert(const value_type &x) {
        return t.insert_unique(x);
    }
    Value& operator[](const key_type &k)
    {
        return (*((insert(value_type(k, Value()))).first)).second;
    }
};
}