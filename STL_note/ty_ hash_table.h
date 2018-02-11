#pragma once
#include "ty_iterator.h"
#include "ty_rb_tree.h"
#include <vector>
#define （ (
#define ） )
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

    node *cur;  // 迭代器目前所指之节点
    hashtable* ht;  // 保持和容器的连结关系(因为可能需要从bucket跳到bucket)
    
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
        // 返回最接近 n 的素数size
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
        resize(num_elements + 1);   //判断是否要重建表格，如果需要就扩充
        return insert_unique_noresize(obj);
    }
    void resize(size_type num_elements_hint)
    {
        // "表格重建与否"的判断原则，是拿元素的个数（把新增元素计入后）和bucket vector的大小来比，
        // 如果前者大于后者，就重建表格 由此可知，每个bucket(list)的最大容量和bucket vector的大小相同
        const size_type old_n = buckets.size();
        if (num_elements_hint > old_n)
        {
            const size_type n = next_size(num_elements_hint);// next prime 
            if (n > old_n)
            {
                vector<node*, Alloc> tmp(n, (node*)0); // 设置新的 buckets
                try
                {
                    // 处理每一个旧的bucket
                    for (size_type bucket = 0; bucket < old_n; ++bucket)
                    {
                        node *first = buckets[bucket];
                        while (first)// 串行还没结束时
                        {
                            size_type new_bucket = bkt_num(first->val, n);// get the pos in new buckets
                            // 1. 令旧bucket指向其所对应的串行的下一个节点(以便迭代处理)
                            buckets[bucket] = first->next;
                            // 2. 3. 将当前节点插入到新的bucket内，成为其对应串行的第一个节点
                            first->next = tmp[new_bucket];
                            tmp[new_bucket] = first;
                            // 4. 回到旧的bucket所指的待处理行，准备处理下一个节点
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
        node *first = buckets[n];   // first 指向buckts对应串行头部

        // 如果buckets[n]已被占用，将进入以下循环
        for (node *cur = first; cur != nullptr; cur = cur->next)
        {
            // 禁止重复键值
            if (equals(get_key(cur->val), get_key(obj)))
                return pair<iterator, bool>(iterator(cur, this), false);
        }

        // 离开以上循环(或根本未进入循环时),first指向buckets所指链表的头部节点
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
        const size_type n = bkt_num(obj);  // 决定 obj 应该位于#n bucket
        node* first = buckets[n];

        for (node *cur = first; cur; cur = cur->next)
        {
            if (equals(get_key(cur->val), get_key(obj)))
            {
                // 如果发现与链表中的某键值相同,就马上插入，然后返回
                node *tmp = new_node(obj);
                tmp->next = cur->next;
                cur->next = tmp;
                ++num_elements;
                return iterator(tmp, this);
            }
        }
        // 表示没有发现重复的键值
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
        // 注意， buckets vector并未释放掉空间，仍保有原来大小
    }

    void copy_from(const hashtable &that)
    {
        // 先清除己方的buckets vector,这操作是调用vector::clear, 将整个容器清空
        this->buckets.clear();
        // 为己方的buckets vector保留空间，使与对方相同
        // 如果己方空间大于对方，就不动，如果己方空间小于对方，就会增大
        this->buckets.reserve(that.buckets.size());
        
        // 从己方的buckets vector尾端开始，插入n个元素，其值为null指针
        // 注意，此时buckets vector为空，所以所谓尾端就是起头处
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