#pragma once


#include <memory.h>
#include <algorithm>
#include <cstring>
#include "ty_function.h"
#include "ty_iterator.h"
#include "ty_rb_tree.h"
#include "ty_alloc.h"

namespace ty {

template <class T>
inline const T& max(const T &a, const T &b)
{
    return a < b ? b : a;
}

template <class T, class Compare>
inline const T& max(const T &a, const T &b, Compare comp)
{
    return comp(a, b) ? b : a;
};

template <class T>
inline const T& min(const T &a, const T &b)
{
    return b < a ? b : a;
}

template <class T, class Compare>
inline const T& min(const T &a, const T &b, Compare comp)
{
    return comp(b, a) ? b : a;
};

template <class InputIterator1, class InputIterator2>
inline bool equal(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2)
{
    for (; first1 != last1; ++first1, ++first2)
        if (*first1 != *first2)
            return false;
    return true;
};

template <class InputIterator1, class InputIterator2, class  BinaryPredicate>
inline bool equal(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, BinaryPredicate pred)
{
    for (; first1 != last1; ++first1, ++first2)
        if (!pred(*first1, *first2))
            return false;
    return true;
};

template <class ForwardIterator, class T>
void fill(ForwardIterator first, ForwardIterator last, const T &value)
{
    for (; first != last; ++first)
        *first = value;
};

template <class OutputIterator, class Size, class T>
OutputIterator fill_n(OutputIterator first, Size n, const T &value)
{
    for (; n > 0; --n, ++first)
        +first = value;
    return first;
};

template <class ForwardIterator1, class ForwardIterator2>
inline void iter_swap(ForwardIterator1 a, ForwardIterator2 b)
{
    __iter_swap(a, b, value_type(a));
};

template <class ForwardIterator1, class ForwardIterator2, class T>
inline void __iter_swap(ForwardIterator1 a, ForwardIterator2 b, T*)
{
    T tmp = *a;
    *a = *b;
    *b = tmp;
};

template <class InputIterator1, class InputIterator2>
bool lexicographical_compare(InputIterator1 first1, InputIterator1 last1, 
                             InputIterator2 first2, InputIterator2 last2)
{
    for (; first1 != last1 && first2 != last2; ++first1, ++first2)
    {
        if (*first1 < *first2)
            return true;
        if (*first2 < *first1)
            return false;
    }
    return first1 == last1 && first2 != last2;
}

template <class InputIterator1, class InputIterator2, class Compare>
bool lexicographical_compare(InputIterator1 first1, InputIterator1 last1,
                             InputIterator2 first2, InputIterator2 last2,
                             Compare comp)
{
    for (; first1 != last1 && first2 != last2; ++first1, ++first2)
    {
        if (comp(*first1, *first2))
            return true;
        if (comp(*first2, *first1))
            return false;
    }
    return first1 == last1 && first2 != last2;
};

inline bool lexicographical_compare(const unsigned char *first1, const unsigned char *last1,
                                    const unsigned char *first2, const unsigned char *last2)
{
    const size_t len1 = last1 - first1;
    const size_t len2 = last2 - first2;
    const int result = ::std::memcmp(first1, first2, min(len1, len2));
    return result != 0 ? result < 0 : len1 < len2;
}

inline bool lexicographical_compare(const char *first1, const char *last1,
                                    const char *first2, const char *last2)
{
    const size_t len1 = last1 - first1;
    const size_t len2 = last2 - first2;
    const int result = ::std::memcmp(first1, first2, min(len1, len2));
    return result != 0 ? result < 0 : len1 < len2;
}

template <class InputIterator1, class InputIterator2>
pair<InputIterator1, InputIterator2>
mismatch(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2)
{
    while (first1 != last1 && *first1 == *first2)
    {
        ++first1;
        ++first2;
    }
    return {first1, first2};
};

template <class InputIterator1, class InputIterator2, class BinaryPredicate>
pair<InputIterator1, InputIterator2>
mismatch(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, BinaryPredicate binary_pred)
{
    while (first1 != last1 && binary_pred(first1, first2))
    {
        ++first1;
        ++first2;
    }
    return {first1, first2};
};

template <class T>
inline void swap(T &a, T &b)
{
    T tmp = a;
    a = b;
    b = tmp;
}

template <class RandomAccessIterator, class OutputIterator, class Distance>
inline OutputIterator __copy_d(RandomAccessIterator first, RandomAccessIterator last,
                               OutputIterator result, Distance*)
{
    for (Distance n = last - first; n > 0; --n, ++result, ++first)
        *result = *first;
    return result;
};

template <class InputIterator, class OutputIterator>
inline OutputIterator __copy(InputIterator first, InputIterator last,
                             OutputIterator result, input_iterator_tag)
{
    for (; first != last; ++first, ++result)
        *result = *first;
    return result;
};

template <class RandomAccessIterator, class OutputIterator>
inline OutputIterator __copy(RandomAccessIterator first, RandomAccessIterator last,
                             OutputIterator result, random_access_iterator_tag)
{
    return __copy_d(first, last, result, distance_type(first));
};

template <class InputIterator, class OutputIterator>
struct __copy_dispatch
{
    OutputIterator operator()(InputIterator first, InputIterator last, OutputIterator result)
    {
        return __copy(first, last, result, iterator_category(first));
    }
};

template <class T>
inline T* __copy_t(const T *first, const T *last, T *result, __true_type)
{
    memmove(result, first, sizeof(T) * (last - first));
    return result + (last - first);
}

template <class T>
inline T* __copy_t(const T *first, const T *last, T *result, __false_type)
{
    return __copy_d(first, last, result, (ptrdiff_t*)0);
}

template <class T>
struct __copy_dispatch<T*, T*>
{
    T* operator()(T *first, T *last, T *result)
    {
        using t = typename __type_traits<T>::has_trivial_assignment_operator;
        return __copy_t(first, last, result, t{});
    }
};

template <class T>
struct __copy_dispatch<const T*, T*>
{
    T* operator()(const T *first, const T *last, T *result)
    {
        using t = typename __type_traits<T>::has_trivial_assignment_operator;
        return __copy_t(first, last, result, t{});
    }
};

template <class InputIterator, class OutputIterator>
inline OutputIterator copy(InputIterator first, InputIterator last, OutputIterator result)
{
    return __copy_dispatch<InputIterator, OutputIterator>()(first, last, result);
};

inline char* copy(const char *first, const char *last, char *result)
{
    memmove(result, first, last - first);
    return result + (last - first);
}

inline wchar_t* copy(const wchar_t *first, const wchar_t *last, wchar_t *result)
{
    memmove(result, first, sizeof(wchar_t) * (last - first));
    return result + (last - first);
}

template <class RandomAccessIterator, class BidirectionalIterator, class Distance>
inline BidirectionalIterator
__copy_backward_d(RandomAccessIterator first, RandomAccessIterator last,
                  BidirectionalIterator result_last, Distance*)
{
    for (Distance n = last - first; n > 0;)
    {
        *(--result_last) = *--(last);
    }
    return result_last;
};

template <class RandomAccessIterator, class BidirectionalIterator>
inline BidirectionalIterator
__copy_backward(RandomAccessIterator first, RandomAccessIterator last,
                BidirectionalIterator result_last, random_access_iterator_tag)
{
    __copy_backward_d(first, last, result_last, distance_type(first));
};

template <class BidirectionalIterator1, class BidirectionalIterator2>
inline BidirectionalIterator2
__copy_backward(BidirectionalIterator1 first, BidirectionalIterator1 last,
                BidirectionalIterator2 result_last, bidirectional_iterator_tag)
{
    while (last != first)
        *(--result_last) = *(--last);
    return result_last;
};

template <class T>
inline T* __copy_backward_t(const T *first, const T *last, T *result_last, __true_type)
{
    memcpy(result_last - (last - first), first, sizeof(T) * (last - first));
    return result_last - (last - first);
}

template <class T>
inline T* __copy_backward_t(const T *first, const T *last, T *result_last, __false_type)
{
    return __copy_backward_d(first, last, result_last, (ptrdiff_t*)0);
}

template <class BidirectionalIterator1, class BidirectionalIterator2>
struct __copy_backward_dispatch
{
    BidirectionalIterator2 operator()(BidirectionalIterator1 first, BidirectionalIterator1 last,
                                      BidirectionalIterator2 result_last)
    {
        return __copy_backward(first, last, result_last, iterator_category(first));
    }
};

template <class T>
struct __copy_backward_dispatch<T*, T*>
{
    T* operator()(T *first, T *last, T *result_last)
    {
        using t = __type_traits<T>::has_trivial_assignment_operator;
        return __copy_backward_t(first, last, result_last, t{});
    }
};

template <class T>
struct __copy_backward_dispatch<const T*, T*>
{
    T* operator()(const T *first, const T *last, T *result_last)
    {
        using t = __type_traits<T>::has_trivial_assignment_operator;
        return __copy_backward_t(first, last, result_last, t{});
    }
};

template <class BidirectionalIterator1, class BidirectionalIterator2>
inline BidirectionalIterator2
copy_backward(BidirectionalIterator1 first, BidirectionalIterator1 last, BidirectionalIterator2 result_last)
{
    return __copy_backward_dispatch<BidirectionalIterator1, BidirectionalIterator2>()(first, last, result_last);
};


}