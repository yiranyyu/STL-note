#pragma once
#include "ty_alloc.h"
#include "ty_rb_tree.h"
#include <functional>
namespace ty
{

template<typename Key, typename Compare = ::std::less<Key>, typename Alloc = alloc>
class set
{
public:
    using key_type      = Key;
    using value_type    = Key;      // value_type and key_type are same for set 
    using key_compare   = Compare;
    using value_compare = Compare;  // value_compare and key_compare use same function
private:
    using rep_type = rb_tree<key_type, value_type, identity<value_type>, key_compare, Alloc>;
    rep_type t; // using rb_tree to represent the set

public:
    using pointer           = typename rep_type::pointer;
    using const_pointer     = typename rep_type::const_pointer;
    using reference         = typename rep_type::reference;
    using const_reference   = typename rep_type::const_reference;
    // using rep_type::const_iterator to represent set iterator, avoid client to write the elements
    using iterator          = typename rep_type::const_iterator; 
    using const_iterator    = typename rep_type::const_iterator;
    using size_type         = typename rep_type::size_type;
    using difference_type   = typename rep_type::difference_type;

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