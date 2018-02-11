#pragma once
#ifndef _TY_UNINITIALIZED_H
#define _TY_UNINITIALIZED_H

#include "ty_construct.h"
#include <memory>

namespace ty {
template <typename ForwardIterator, typename Size, typename T>
inline ForwardIterator uninitialized_fill_n(ForwardIterator first, Size size, const T &x)
{
    return __uninitialized_fill_n(first, size, x, value_type(first));
}

template <typename ForwardIterator, typename Size, typename T, typename T1>
inline ForwardIterator __uninitialized_fill_n(ForwardIterator first, Size size, const T &x, T1*)
{
    // is Plain Old Data so called scalar types
    // POD must have trivial ctor/dtor/copy/assignment function, so we can take the most 
    // effective way to init the memory. For non-POD types, need to take the most safe way
    // to init the memory
    typedef typename __type_traits<T1>::is_POD_type is_POD;
    return __uninitialized_fill_n_aux(first, size, x, is_POD());
}

// This function works if copy ctor same as assigment and dtor is tirvial
// 执行流程藉由参数推导机制转到以下函数
template <typename ForwardIterator, typename Size, typename T>
inline ForwardIterator __uninitialized_fill_n_aux(ForwardIterator first, Size n, const T &x, __true_type)
{
    return fill_n(first, n, x);// assign work to higher order function
}

// This function works if T is not POD type
template <typename ForwardIterator, typename Size, typename T>
inline ForwardIterator __uninitialized_fill_n_aux(ForwardIterator first, Size n, const T &x, __false_type)
{
    ForwardIterator current = first;
    try
    {
        for (; n > 0; --n, ++current)
        {
            construct(&*current, x);
        }
        return current;
    }
    catch (...)
    {
        destroy(first, current); // [first, current]
        throw;
    }
    return cur;
}

template <typename InputIterator, typename ForwardIterator>
inline ForwardIterator uninitialized_copy(InputIterator first, InputIterator last, ForwardIterator result)
{
    return __uninitialized_copy(first, last, result, value_type(result));
}

template <typename InputIterator, typename ForwardIterator, typename T>
inline ForwardIterator __uninitialized_copy(InputIterator first, InputIterator last, ForwardIterator result, T*)
{
    typedef typename __type_traits<T>::is_POD_type is_POD;
    // try to use is_POD() to let the compiler find the function to call 
    return __uninitialized_copy_aux(first, last, result, is_POD());
}

template <typename InputIterator, typename ForwardIterator>
inline ForwardIterator
__uninitialized_copy_aux(InputIterator first,
                         InputIterator last, ForwardIterator result, __false_type)
{
    ForwardIterator current = result;
    try
    {
        for (; first != last; ++first, ++current)
            construct(&*current, *first);
        return current;
    }
    catch (...)
    {
        destroy(result, current);// destroy all the elements already constructed
        throw;
    }
}

template <typename InputIterator, typename ForwardIterator>
inline ForwardIterator
__uninitialized_copy_aux(InputIterator first,
                         InputIterator last, ForwardIterator result, __true_type)
{
    return copy(first, last, result);
}

template<>
inline char* uninitialized_copy(const char *first, const char *last, char *result)
{
    memmove(result, first, last - first);
    return result + (last - first);
}

template<>
inline wchar_t* uninitialized_copy(const wchar_t *first, const wchar_t *last, wchar_t *result)
{
    memmove(result, first, sizeof(wchar_t) * (last - first));
    return result + (last - first);
}

template <typename ForwardIterator, typename T>
inline ForwardIterator
uninitialized_fill(ForwardIterator first, ForwardIterator last, const T &x)
{
    return __uninitialized_fill(first, last, x, value_type(x));
}

template <typename ForwardIterator, typename T, typename T1>
inline ForwardIterator
__uninitialized_fill(ForwardIterator first, ForwardIterator last, const T &x, T1*)
{
    typedef typename __type_traits<T1>::is_POD_type is_POD;
    return __uninitialized_fill_aux(first, last, x, is_POD());
}

template <typename ForwardIterator, typename T>
inline ForwardIterator
__uninitialized_fill_aux(ForwardIterator first, ForwardIterator last, const T &x, __true_type)
{
    fill(first, last, x);// call STL algorithm
}

template <typename ForwardIterator, typename T>
inline ForwardIterator
__uninitialized_fill_aux(ForwardIterator first, ForwardIterator last, const T &x, __false_type)
{
    ForwardIterator current = first;
    try
    {
        for (; current != last; ++current)
            construct(&*current, x);
        return current;
    }
    catch (...)
    {
        destroy(first, last);
        throw;
    }
}
}
#endif // !_TY_UNINITIALIZED_H
