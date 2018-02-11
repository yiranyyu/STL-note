#pragma once
#ifndef _TY_CONSTRUCT_H
#define _TY_CONSTRUCT_H

#include <new.h> // for placement new
#include "ty_iterator.h" // for iterator_traits
#include "ty_type_traits.h"

namespace ty {
template <typename T1, typename T2>
inline void construct(T1 *ptr, const T2 &value)
{
    new (ptr) T1(value);
}

template <typename T>
inline void destroy(T *pointer)
{
    pointer->~T();
}

template <typename ForwardIterator>
inline void destroy(ForwardIterator first, ForwardIterator last)
{
    __destroy(first, last, value_type(first));
}

template <typename ForwardIterator, typename T>
inline void __destroy(ForwardIterator first, ForwardIterator last, T*)
{
    typedef typename __type_traits<T>::has_trivial_destructor trivial_destructor;
    __destroy_aux(first, last, trivial_destructor());
}

// for types with non-trivial dtor
template <typename ForwardIterator>
inline void __destroy_aux(ForwardIterator first, ForwardIterator last, __false_type)
{
    for (; first < last; ++first)
        destroy(&*first);
}

// for types with trivial dtor
template <typename ForwardIterator>
inline void __destroy_aux(ForwardIterator first, ForwardIterator last, __true_type)
{
    // Do Nothing
}

// specialization of char * and wchar_t * type Iterator
inline void destroy(char *, char *) {}
inline void destroy(wchar_t *, wchar_t *) {}

}// namespace ty

#endif // !_TY_CONSTRUCT_H
