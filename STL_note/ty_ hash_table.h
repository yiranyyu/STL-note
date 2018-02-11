#pragma once
#include "ty_iterator.h"
#include "ty_rb_tree.h"
#include <vector>
#define �� (
#define �� )
namespace ty
{
template <typename T, typename Alloc>
using vector = std::vector<T, Alloc>;

static const int __stl_num_primes = 28;
static const unsigned long __stl_prime_list[__stl_num_primes] = {
    53,         97,         193,        389,        769,
    1543,       3079,       6151,       12289,      24593,
    49157,      98317,      196613,     393241,     786433,
    1572869,    3145739,    6291469,    12582917,   25165843,
    50331653,   100663319,  201326611,  402653189,  805306457,
    1610612741, 3221225473UL, 4294967291UL
};
inline unsigned long __stl_next_prime(unsigned long n)
{
    const auto first = __stl_prime_list;
    const auto last = first + __stl_num_primes;
    auto pos = first;       //<---------------ERROR, fix it if complete the genetic algorithm lower_bound
    //const auto pos = lower_bound(first, last, n);
    return pos == last ? *(last - 1) : *pos;
}


template <typename Value>
struct __hashtable_node
{
    __hashtable_node* next;
    Value val;
};

template <typename Value, typename Key, typename HashFcn,
          typename ExtractKey, typename EqualKey, typename Alloc = alloc> class hashtable;
template <typename Value, typename Key, typename HashFcn,
          typename ExtractKey, typename EqualKey, typename Alloc>
struct __hashtable_iterator
{
    using hashtable         = hashtable<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>;
    using iterator          = __hashtable_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>;
    using node              = __hashtable_node<Value>;

    using iterator_category = forward_iterator_tag;
    using difference_type   = ::std::ptrdiff_t;
    using size_type     = size_t;
    using value_type    = Value;
    using reference     = Value&;
    using pointer       = Value*;

    node *cur;  // ������Ŀǰ��ָ֮�ڵ�
    hashtable* ht;  // ���ֺ������������ϵ(��Ϊ������Ҫ��bucket����bucket)
    
    __hashtable_iterator(node *n, hashtable *table) : cur(n), ht(table) {};
    __hashtable_iterator() {}
    reference operator*() const { return cur->val; }
    pointer operator->() const { return &(operator*()); }
    bool operator==(const iterator &that) const { return that.cur == this->cur; }
    bool operator!=(const iterator &that) const { return that.cur != this->cur; }
    iterator& operator++()
    {
        const node* old = cur;
        cur = cur->next;
        if (cur == nullptr)
        {
            size_type bucket = ht->bkt_num(old->val);
            while (cur == nullptr && ++bucket < ht->buckets.size())
                cur = ht->buckets[bucket];
        }
        return *this;
    }
    iterator operator++(int)
    {
        iterator tmp = *this;
        ++*this;
        return tmp;
    }
};

template <typename Value, typename Key, typename HashFcn,
    typename ExtractKey, typename EqualKey, typename Alloc> 
class hashtable
{
public:
    using hasher    = HashFcn;  // rename the type parameter
    using key_equal = EqualKey; // rename the type parameter
    using size_type = size_t;
    using value_type = Value;
    using key_type = Key;
    using iterator = __hashtable_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>;

private:
    hasher hash;
    key_equal equals;
    ExtractKey get_key;

    using node = __hashtable_node<Value>;
    using node_allocator = simple_alloc<node, Alloc>;
    
    vector<node*, Alloc> buckets;
    size_type num_elements;

public:
    hashtable(size_type n, const hasher &hf, const key_equal &eql)
        :hash(hf), equals(eql), get_key(ExtractKey()), num_elements(0) 
    {
        initialize_buckets(n);
    }
    void initialize_buckets(size_type n)
    {
        // ������ӽ� n ������size
        const size_type n_buckets = next_size(n);
        buckets.reserve(n_buckets);
        buckets.insert(buckets.begin(), n_buckets, (node*)0);
        num_elements = 0;
    }
    size_type next_size(size_type n) const { return __stl_next_prime(n); }
    
    // number of bucket is the size of buckets vector
    size_type bucket_count() const { return buckets.size(); }
    
    size_type max_buck_count() const {
        return __stl_prime_list[__stl_num_primes - 1];  // the return value is always 4294967291UL
    }
    pair<iterator, bool> insert_unique(const value_type &obj)
    {
        resize(num_elements + 1);   //�ж��Ƿ�Ҫ�ؽ���������Ҫ������
        return insert_unique_noresize(obj);
    }
    void resize(size_type num_elements_hint)
    {
        // "����ؽ����"���ж�ԭ������Ԫ�صĸ�����������Ԫ�ؼ���󣩺�bucket vector�Ĵ�С���ȣ�
        // ���ǰ�ߴ��ں��ߣ����ؽ���� �ɴ˿�֪��ÿ��bucket(list)�����������bucket vector�Ĵ�С��ͬ
        const size_type old_n = buckets.size();
        if (num_elements_hint > old_n)
        {
            const size_type n = next_size(num_elements_hint);// next prime 
            if (n > old_n)
            {
                vector<node*, Alloc> tmp(n, (node*)0); // �����µ� buckets
                try
                {
                    // ����ÿһ���ɵ�bucket
                    for (size_type bucket = 0; bucket < old_n; ++bucket)
                    {
                        node *first = buckets[bucket];
                        while (first)// ���л�û����ʱ
                        {
                            size_type new_bucket = bkt_num(first->val, n);// get the pos in new buckets
                            // 1. ���bucketָ��������Ӧ�Ĵ��е���һ���ڵ�(�Ա��������)
                            buckets[bucket] = first->next;
                            // 2. 3. ����ǰ�ڵ���뵽�µ�bucket�ڣ���Ϊ���Ӧ���еĵ�һ���ڵ�
                            first->next = tmp[new_bucket];
                            tmp[new_bucket] = first;
                            // 4. �ص��ɵ�bucket��ָ�Ĵ������У�׼��������һ���ڵ�
                            first = buckets[bucket];
                        }
                    }
                    buckets.swap(tmp);
                }
            }
        }
    }
    pair<iterator, bool> insert_unique_noresize(const value_type &obj)
    {
        const size_type n = bkt_num(obj);
        node *first = buckets[n];   // first ָ��buckts��Ӧ����ͷ��

        // ���buckets[n]�ѱ�ռ�ã�����������ѭ��
        for (node *cur = first; cur != nullptr; cur = cur->next)
        {
            // ��ֹ�ظ���ֵ
            if (equals(get_key(cur->val), get_key(obj)))
                return pair<iterator, bool>(iterator(cur, this), false);
        }

        // �뿪����ѭ��(�����δ����ѭ��ʱ),firstָ��buckets��ָ�����ͷ���ڵ�
        node *tmp = new_node(obj);
        tmp->next = first;
        buckets[n] = tmp;
        ++num_elements;
        return pair<iterator, bool>(iterator(tmp, this), true);
    }
    iterator insert_equal(const value_type &obj)
    {
        resize(num_elements + 1);
        return insert_equal_noresize(obj);
    }
    iterator insert_equal_noresize(const value_type &obj)
    {
        const size_type n = bkt_num(obj);  // ���� obj Ӧ��λ��#n bucket
        node* first = buckets[n];

        for (node *cur = first; cur; cur = cur->next)
        {
            if (equals(get_key(cur->val), get_key(obj)))
            {
                // ��������������е�ĳ��ֵ��ͬ,�����ϲ��룬Ȼ�󷵻�
                node *tmp = new_node(obj);
                tmp->next = cur->next;
                cur->next = tmp;
                ++num_elements;
                return iterator(tmp, this);
            }
        }
        // ��ʾû�з����ظ��ļ�ֵ
        node *tmp = new_node(obj);
        tmp->next = first;
        buckets[n] = tmp;
        ++num_elements;
        return iterator(tmp, this);
    }

    // version 1.0
    size_type bkt_num(const value_type &obj, size_type n) const
    {
        return bkt_num_key(get_key(obj), n);    // call version 4.0
    }

    // version 2.0
    size_type bkt_num(const value_type &obj) const
    {
        return bkt_num_key(get_key(obj));       // call version 3.0
    }

    // version 3.0
    size_type bkt_num_key(const key_type &key) const
    {
        return bkt_num_key(key, buckets.size());// call version 4.0
    }

    // version 4.0
    size_type bkt_num_key(const key_type &key, size_t n) const
    {
        return hash(key) % n;
    }

    void clear()
    {
        // for every bucket
        for (size_type i = 0; i < buckets.size(); ++i)
        {
            node *cur = buckets[i];

            // for every link node in the bucket
            while (cur != nullptr)
            {
                node *next = cur->next;
                delete_node(cur);
                cur = next;
            }
            buckets[i] = 0;
        }
        num_elements = 0;
        // ע�⣬ buckets vector��δ�ͷŵ��ռ䣬�Ա���ԭ����С
    }

    void copy_from(const hashtable &that)
    {
        // �����������buckets vector,������ǵ���vector::clear, �������������
        this->buckets.clear();
        // Ϊ������buckets vector�����ռ䣬ʹ��Է���ͬ
        // ��������ռ���ڶԷ����Ͳ�������������ռ�С�ڶԷ����ͻ�����
        this->buckets.reserve(that.buckets.size());
        
        // �Ӽ�����buckets vectorβ�˿�ʼ������n��Ԫ�أ���ֵΪnullָ��
        // ע�⣬��ʱbuckets vectorΪ�գ�������νβ�˾�����ͷ��
        this->buckets.insert(this->buckets.end(), that.buckets.size(), (node*)0);
        try
        {
            for (size_type i = 0; i < that.buckets.size(); ++i)
            {
                if (const node *cur = that.buckets[i])
                {
                    node *copy = new_node(cur->val);
                    this->buckets[i] = copy;

                    // for one bucket list, copy it's every node
                    for (node *next = cur->next; next; cur = next, next = cur->next)
                    {
                        copy->next = new_node(next->val);
                        copy = copy->next;
                    }
                }
            }
            num_elements = that.num_elements;
        }
        catch (...) { this->clear(); }
    }
    iterator find(const key_type &key)
    {
        const size_type n = bkt_num_key(key);
        node *first;
        for (first = buckets[n]; first && !equals(get_key(first->val), key); first = first->next) {}
        return iterator(first, this);
    }

    size_type count(const key_type &key) const
    {
        const size_type n = bkt_num_key(key);
        size_type result = 0;
        for (const node *cur = buckets[n]; cur; cur = cur->next)
            if (equals(get_key(cur->val), key)) 
                ++result;
        return result;
    }
    //... other functions
private:
    node *new_node(const value_type &obj)
    {
        node *n = node_allocator::allocate();
        n->next = nullptr;
        try
        {
            construct(&n->val, obj);
            return n;
        }
        catch (...)
        {
            node_allocator::deallocate(n);
        }
    }
    void delete_node(node *n)
    {
        destroy(&n->val);
        node_allocator::deallocate(n);
    }
};
}