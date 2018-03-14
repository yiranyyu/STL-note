#pragma once

#ifndef _TY_ITERATOR_H
#define _TY_ITERATOR_H


#include <cstddef>
namespace ty {

struct input_iterator_tag {};
struct output_iterator_tag {};
struct forward_iterator_tag : public input_iterator_tag {};
struct bidirectional_iterator_tag : public forward_iterator_tag {};
struct random_access_iterator_tag : public bidirectional_iterator_tag {};

// any iterator class inheriting this class template will be compatible the STL
template <typename Category,
    typename T,
    typename Distance = ptrdiff_t,
    typename Pointer = T*,
    typename Reference = T&>
    struct iterator
{
    typedef Category        iterator_category;
    typedef T               value_type;
    typedef Distance        difference_type;
    typedef Pointer         pointer;
    typedef Reference       referenct;
};

template <typename T>
struct iterator_traits
{
    typedef typename T::iterator_category   iterator_category;
    typedef typename T::value_type          value_type;
    typedef typename T::difference_type     difference_type;
    typedef typename T::pointer             pointer;
    typedef typename T::reference           reference;
};

// partial specialization of primitive pointer type
template <typename T>
struct iterator_traits<T*>
{
    typedef random_access_iterator_tag  iterator_category;
    typedef T                           value_type;
    typedef ptrdiff_t                   difference_type;
    typedef T*                          pointer;
    typedef T&                          reference;
};

// partial specialization of primitive pointer-to-const type
template <typename T>
struct iterator_traits<const T*>
{
    typedef random_access_iterator_tag  iterator_category;
    typedef T                           value_type;
    typedef ptrdiff_t                   difference_type;
    typedef const T*                    pointer;
    typedef const T&                    reference;
};

template <class Iterator>
inline typename iterator_traits<Iterator>::iterator_category
iterator_category(const Iterator&)
{
    typedef typename iterator_traits<Iterator>::iterator_category category;
    return category();
}

// easy to get difference_type of an iterator_type
// return object of type difference_type*
template <typename Iterator>
inline typename iterator_traits<Iterator>::difference_type*
distance_type(const Iterator&)
{
    return static_cast<typename iterator_traits<Iterator>::difference_type *>(0);
}

// easy to get value_type of iterator_type
// return object of type value_type*
template <typename Iterator>
inline typename iterator_traits<Iterator>::value_type*
value_type(const Iterator&)
{
    return static_cast<typename iterator_traits<Iterator>::value_type*>(0);
}
template <typename InputIterator, typename Distance>
inline void __advance(InputIterator &i, Distance n, input_iterator_tag)
{
    while (n--)
        ++i;
}

template <typename BidirectionalIterator, typename Distance>
inline void __advance(BidirectionalIterator &i, Distance n, bidirectional_iterator_tag)
{
    if (n >= 0)
        while (n--) ++i;
    else
        while (n++) --i;
}

template <typename RandomAccessIterator, typename Distance>
inline void __advance(RandomAccessIterator &i, Distance n, random_access_iterator_tag)
{
    i += n;
}

// follow the name principle of STL, name the type parameter with the
// lowest level iterator type certain algorithms can handle with
template <typename InputIterator, typename Distance>
inline void advance(InputIterator &i, Distance n)
{
    __advance(i, n, iterator_category(i));
}

template <typename InputIterator>
inline typename iterator_traits<InputIterator>::difference_type
__distance(InputIterator first, InputIterator last,
           input_iterator_tag)
{
    typename iterator_traits<InputIterator>::difference_type n = 0;
    while (first != last)
    {
        ++first;
        ++n;
    }
    return n;
}

template <typename RandomAccessIterator>
inline typename iterator_traits<RandomAccessIterator>::difference_type
__distance(RandomAccessIterator first, RandomAccessIterator last,
           random_access_iterator_tag)
{
    return last - first;
}

template <typename InputIterator>
inline typename iterator_traits<InputIterator>::difference_type
distance(InputIterator first, InputIterator last)
{
    __distance(first, last, iterator_category(first));
}

}// namespace ty

#endif // !_TY_ITERATOR_H