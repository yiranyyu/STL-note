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

template<typename Key, typename Compare = less<Key>, typename Alloc = alloc>
class set
{
public:
    using key_type      = Key;
    using value_type    = Key;      // value_type and key_type are same for set 
    using key_compare   = Compare;
    using value_compare = Compare;  // value_compare and key_compare use same function
private:
    template<typename T>
    struct identity
    {
        const T& operator()(const T &x) const { return x; }
    };

    using rep_type = rb_tree<key_type, value_type, identity<value_type>, key_compare, Alloc>;
    rep_type t; // using rb_tree to represent the set

public:
    using pointer           = rep_type::pointer;
    using const_pointer     = rep_type::const_pointer;
    using reference         = rep_type::reference;
    using const_reference   = rep_type::const_reference;
    // using rep_type::const_iterator to represent set iterator, avoid client to write the elements
    using iterator          = rep_type::const_iterator; 
    using const_iterator    = rep_type::const_iterator;
    using size_type         = rep_type::size_type;
    using difference_type   = rep_type::difference_type;

    set() :t(Compare()) {}
    explicit set(const Compare &comp) :t(comp) {}

    key_compare key_comp() const { return t.key_comp(); }
    value_compare value_comp() const { return t.key_comp(); }
    iterator begin() const { return t.begin(); }
    iterator end() const { return t.end(); }
    bool empty() const { return t.empty(); }
    size_type size() const { return t.size(); }
    size_type max_size() const { return t.max_size(); }

    using pair_iterator_bool = pair<iterator, bool>;
    pair_iterator_bool insert(const value_type &x)
    {
        pair<typename rep_type::iterator, bool> p = t.insert_unique(x);
        return pair_iterator_bool(p.first, p.second);
    }
};

}