#pragma once

#include "ty_algobase.h"
#include "ty_heap.h"
#include "ty_buffer.h"


// TODO build the real reverse_iterator
namespace ty{
template <class T>
struct reverse_iterator{bool operator==(const reverse_iterator &that){return true;} int base(){return 1;}};
}


namespace ty
{

const int __stl_threshold = 16;

template <class InputIterator1, class InputIterator2, class OutputIterator>
OutputIterator set_union(InputIterator1 first1, InputIterator1 last1,
                         InputIterator2 first2, InputIterator2 last2, 
                         OutputIterator result)
{
    while (first1 != last1 && first2 != last2)
    {
        if (*first1 < *first2)
        {
            *result = *first1;
            ++first1;
        }
        else if (*first2 < *first1)
        {
            *result = *first2;
            ++first2;
        }
        else
        {
            *result = *first1;
            ++first1;
            ++first2;
        }
    }

    return copy(first2, last2, copy(first1, last1, result));
}

template <class InputIterator1, class InputIterator2, class OutputIterator, class Compare>
OutputIterator set_union(InputIterator1 first1, InputIterator1 last1,
                         InputIterator2 first2, InputIterator2 last2,
                         OutputIterator result, Compare cmp)
{
    while (first1 != last1 && first2 != last2)
    {
        if (cmp(*first1, *first2))
        {
            *result = *first1;
            ++first1;
        }
        else if (cmp(*first2, *first1))
        {
            *result = *first2;
            ++first2;
        }
        else
        {
            *result = *first1;
            ++first1;
            ++first2;
        }
    }

    return copy(first2, last2, copy(first1, last1, result));
}

template <class InputIterator1, class InputIterator2, class OutputIterator>
OutputIterator set_intersection(InputIterator1 first1, InputIterator1 last1,
                                InputIterator2 first2, InputIterator2 last2,
                                OutputIterator result)
{
    while (first1 != last1 && first2 != last2)
    {
        if (*first1 < *first2)
            ++first1;
        else if (*first2 < *first1)
            ++first2;
        else
        {
            *result = *first1;
            ++first1;
            ++first2;
            ++result;
        }
    }
    return result;
};

template <class InputIterator1, class InputIterator2, class OutputIterator, class Compare>
OutputIterator set_intersection(InputIterator1 first1, InputIterator1 last1,
                                InputIterator2 first2, InputIterator2 last2,
                                OutputIterator result, Compare cmp)
{
    while (first1 != last1 && first2 != last2)
    {
        if (cmp(*first1, *first2))
            ++first1;
        else if (cmp(*first2, *first1))
            ++first2;
        else
        {
            *result = *first1;
            ++first1;
            ++first2;
            ++result;
        }
    }
    return result;
};

template <class InputIterator1, class InputIterator2, class OutputIterator>
OutputIterator set_difference(InputIterator1 first1, InputIterator1 last1,
                              InputIterator2 first2, InputIterator2 last2,
                              OutputIterator result)
{
    while (first1 != last1 && first2 != last2)
    {
        if (*first1 < *first2)
        {
            *result = *first1;
            ++first1;
            ++result;
        }
        else if (*first2 < *first1)
            ++first2;
        else
        {
            ++first1;
            ++first2;
        }
    }
    return copy(first1, last1, result);
};


template <class InputIterator1, class InputIterator2, class OutputIterator, class Compare>
OutputIterator set_difference(InputIterator1 first1, InputIterator1 last1,
                              InputIterator2 first2, InputIterator2 last2,
                              OutputIterator result, Compare cmp)
{
    while (first1 != last1 && first2 != last2)
    {
        if (cmp(*first1, *first2))
        {
            *result = *first1;
            ++first1;
            ++result;
        }
        else if (cmp(*first2, *first1))
            ++first2;
        else
        {
            ++first1;
            ++first2;
        }
    }
    return copy(first1, last1, result);
};

template <class InputIterator1, class InputIterator2, class OutputIterator>
OutputIterator set_symmetric_difference(InputIterator1 first1, InputIterator1 last1,
                                        InputIterator2 first2, InputIterator2 last2,
                                        OutputIterator result)
{
    while (first1 != last1 && first2 != last2)
    {
        if (*first1 < *first2)
        {
            *result = *first1;
            ++first1;
            ++result;
        }
        else if (*first2 < *first1)
        {
            *result = *first2;
            ++first2;
            ++result;
        }
        else
        {
            ++first1;
            ++first2;
        }
    }
    return copy(first2, last2, copy(first1, last1, result));
};


template <class InputIterator1, class InputIterator2, class OutputIterator, class Compare>
OutputIterator set_symmetric_difference(InputIterator1 first1, InputIterator1 last1,
                                        InputIterator2 first2, InputIterator2 last2,
                                        OutputIterator result, Compare cmp)
{
    while (first1 != last1 && first2 != last2)
    {
        if (cmp(*first1, *first2))
        {
            *result = *first1;
            ++first1;
            ++result;
        }
        else if (cmp(*first2, *first1))
        {
            *result = *first2;
            ++first2;
            ++result;
        }
        else
        {
            ++first1;
            ++first2;
        }
    }
    return copy(first2, last2, copy(first1, last1, result));
};

template <class ForwardIterator>
ForwardIterator adjacent_find(ForwardIterator first, ForwardIterator last)
{
    if (first == last)
        return last;
    auto next = first;
    while (++next != last)
    {
        if (*first == *next)
            return first;
        first = next;
    }
    return last;
}

template <class ForwardIterator, class BinaryPredicate>
ForwardIterator adjacent_find(ForwardIterator first, ForwardIterator last, BinaryPredicate binary_pred)
{
    if (first == last)
        return last;
    auto next = first;
    while (++next != last)
    {
        if (binary_pred(*first, *next))
            return first;
        first = next;
    }
    return last;
}

template <class InputIterator, class T>
typename iterator_traits<InputIterator>::difference_type
count(InputIterator first, InputIterator last, const T &value)
{
    typename iterator_traits<InputIterator>::difference_type n = 0;
    for (; first != last; ++first)
    {
        if (*first == value)
            ++n;
    }
    return n;
};

template <class InputIterator, class T, class Size>
void count(InputIterator first, InputIterator last, const T &value, Size &n)
{
    for (; first != last; ++first)
    {
        if (*first == value)
            ++n;
    }
};

template <class InputIterator, class Predicate>
typename iterator_traits<InputIterator>::difference_type
count_if(InputIterator first, InputIterator last, Predicate pred)
{
    typename iterator_traits<InputIterator>::difference_type n = 0;
    for (; first != last; ++first)
    {
        if (pred(*first))
            ++n;
    }
    return n;
};

template <class InputIterator, class T>
InputIterator find(InputIterator first, InputIterator last, const T &value)
{
    while (first != last && *first != value)
        ++first;
    return first;
};

template <class InputIterator, class Predicate>
InputIterator find_if(InputIterator first, InputIterator last, Predicate pred)
{
    while (first != last && !pred(*first))
        ++first;
    return first;
};

template <class ForwardIterator1, class ForwardIterator2>
ForwardIterator1 __find_end(ForwardIterator1 first1, ForwardIterator1 last1,
                            ForwardIterator2 first2, ForwardIterator2 last2,
                            forward_iterator_tag, forward_iterator_tag)
{
    if (first2 == last2)
        return last1;
    ForwardIterator1 result = last1;
    while (true)
    {
        ForwardIterator1 new_result = search(first1, last1, first2, last2);
        if (new_result == last1) // not found
            return result;  // return prev result
        else
        {
            result = new_result;
            first1 = new_result;
            ++first1;
        }
    }
};

template <class ForwardIterator1, class ForwardIterator2, class Distance1, class Distance2>
ForwardIterator1 __search(ForwardIterator1 first1, ForwardIterator1 last1,
                          ForwardIterator2 first2, ForwardIterator2 last2,
                          Distance1*, Distance2*)
{
    Distance1 d1 = distance(first1, last1);
    Distance2 d2 = distance(first2, last2);
    if (d1 < d2)
        return last1;

    ForwardIterator1 current1 = first1;
    ForwardIterator2 current2 = first2;
    while (current2 != last2)
    {
        if (*current1 == *current2)
        {
            ++current1;
            ++current2;
        }
        else
        {
            if (d1 == d2)
                return last1;
            current1 = ++first1;// use first1 to point the start position of searching
            current2 = first2;
            --d1;
        }
    }
    return first1;
};

template <class ForwardIterator1, class ForwardIterator2>
inline ForwardIterator1 search(ForwardIterator1 first1, ForwardIterator1 last1,
                               ForwardIterator2 first2, ForwardIterator2 last2)
{
    return __search(first1, last1, first2, last2, distance_type(first1), distance_type(first2));
};

template <class ForwardIterator1, class ForwardIterator2, class BinaryPredicate, class Distance1, class Distance2>
ForwardIterator1 __search(ForwardIterator1 first1, ForwardIterator1 last1,
                          ForwardIterator2 first2, ForwardIterator2 last2,
                          BinaryPredicate binary_pred, Distance1*, Distance2*)
{
    Distance1 d1 = distance(first1, last1);
    Distance2 d2 = distance(first2, last2);
    if (d1 < d2)
        return last1;

    ForwardIterator1 current1 = first1;
    ForwardIterator2 current2 = first2;
    while (current2 != last2)
    {
        if (binary_pred(*current1, *current2))
        {
            ++current1;
            ++current2;
        }
        else
        {
            if (d1 == d2)
                return last1;
            current1 = ++first1;// use first1 to point the start position of searching
            current2 = first2;
            --d1;
        }
    }
    return first1;
};

template <class ForwardIterator1, class ForwardIterator2, class BinaryPredicate>
inline ForwardIterator1 search(ForwardIterator1 first1, ForwardIterator1 last1,
                               ForwardIterator2 first2, ForwardIterator2 last2, BinaryPredicate binary_pred)
{
    return __search(first1, last1, first2, last2, binary_pred, distance_type(first1), distance_type(first2));
};

template <class BidirectionalIterator1, class BidirectionalIterator2>
BidirectionalIterator1
__find_end(BidirectionalIterator1 first1, BidirectionalIterator1 last1,
           BidirectionalIterator2 first2, BidirectionalIterator2 last2,
           bidirectional_iterator_tag, bidirectional_iterator_tag)
{
    using revIter1 = reverse_iterator<BidirectionalIterator1>;
    using revIter2 = reverse_iterator<BidirectionalIterator2>;

    revIter1 rLast1(first1);
    revIter2 rLast2(first2);
    revIter1 rResult = search(revIter1(last1), rLast1, revIter2(last2), rLast2);

    if (rResult == rLast1)
        return last1;
    else
    {
        BidirectionalIterator1 result = rResult.base();
        advance(result, -distance(first2, last2));  // set position back to start
        return result;
    }
};


// return the last occurrence of sequence [first2, last2) in [first1, last1)
template <class ForwardIterator1, class ForwardIterator2>
inline ForwardIterator1
find_end(ForwardIterator1 first1, ForwardIterator1 last1,
         ForwardIterator2 first2, ForwardIterator2 last2)
{
    using category1 = typename iterator_traits<ForwardIterator1>::iterator_category;
    using category2 = typename iterator_traits<ForwardIterator2>::iterator_category;
    return __find_end(first1, last1, first2, last2, category1(), category2());
};

template <class ForwardIterator1, class ForwardIterator2, class BinaryPredicate>
ForwardIterator1 __find_end(ForwardIterator1 first1, ForwardIterator1 last1,
                            ForwardIterator2 first2, ForwardIterator2 last2,
                            BinaryPredicate pred, forward_iterator_tag, forward_iterator_tag)
{
    if (first2 == last2)
        return last1;
    ForwardIterator1 result = last1;
    while (true)
    {
        ForwardIterator1 new_result = search(first1, last1, first2, last2, pred);
        if (new_result == last1) // not found
            return result;  // return prev result
        else
        {
            result = new_result;
            first1 = new_result;
            ++first1;
        }
    }
};

template <class BidirectionalIterator1, class BidirectionalIterator2, class BinaryPredicate>
BidirectionalIterator1
__find_end(BidirectionalIterator1 first1, BidirectionalIterator1 last1,
           BidirectionalIterator2 first2, BidirectionalIterator2 last2,
           BinaryPredicate pred, bidirectional_iterator_tag, bidirectional_iterator_tag)
{
    using revIter1 = reverse_iterator<BidirectionalIterator1>;
    using revIter2 = reverse_iterator<BidirectionalIterator2>;

    revIter1 rLast1(first1);
    revIter2 rLast2(first2);
    revIter1 rResult = search(revIter1(last1), rLast1, revIter2(last2), rLast2, pred);

    if (rResult == rLast1)
        return last1;
    else
    {
        BidirectionalIterator1 result = rResult.base();
        advance(result, -distance(first2, last2));  // set position back to start
        return result;
    }
};


// return the last occurrence of sequence [first2, last2) in [first1, last1)
template <class ForwardIterator1, class ForwardIterator2, class BinaryPredicate>
inline ForwardIterator1
find_end(ForwardIterator1 first1, ForwardIterator1 last1,
         ForwardIterator2 first2, ForwardIterator2 last2, BinaryPredicate pred)
{
    using category1 = typename iterator_traits<ForwardIterator1>::iterator_category;
    using category2 = typename iterator_traits<ForwardIterator2>::iterator_category;
    return __find_end(first1, last1, first2, last2, pred, category1(), category2());
};

template <class InputIterator, class ForwardIterator>
InputIterator find_first_of(InputIterator first1, InputIterator last1,
                            ForwardIterator first2, ForwardIterator last2)
{
    for (; first1 != last1; ++first1)
    {
        for (ForwardIterator iter = first2; iter != last2; ++iter)
        {
            if (*first1 == *iter)
                return first1;
        }
    }
    return last1;
}

template <class InputIterator, class ForwardIterator, class BinaryPredicate>
InputIterator find_first_of(InputIterator first1, InputIterator last1,
                            ForwardIterator first2, ForwardIterator last2,
                            BinaryPredicate cmp)
{
    for (; first1 != last1; ++first1)
    {
        for (ForwardIterator iter = first2; iter != last2; ++iter)
        {
            if (cmp(*first1, *iter))
                return first1;
        }
    }
    return last1;
}

template <class InputIterator, class Function>
Function for_each(InputIterator first, InputIterator last, Function func)
{
    for (; first != last; ++first)
        func(*first);
    return func;
};

template <class ForwardIterator, class Generator>
void generate(ForwardIterator first, ForwardIterator last, Generator gen)
{
    for (; first != last; ++first)
        *first = gen();
};

template <class OutputIterator, class Size, class Generator>
OutputIterator generate_n(OutputIterator first, Size n, Generator gen)
{
    for (; n > 0; --n, ++first)
        *first = gen();
    return first;
};

// includes
// if the sequences are sorted with less, than use the less edition (default editon)
// if the sequences are sorted with greater, than use greater to call

template <class InputIterator1, class InputIterator2>
bool includes(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, InputIterator2 last2)
{
    while (first1 != last1 && first2 != last2)
    {
        if (*first2 < *first1)
            return false;
        else if (*first1 < *first2)
            ++first1;
        else
        {
            ++first1;
            ++first2;
        }
    }
    return first2 == last2;
};

template <class InputIterator1, class InputIterator2, class Compare>
bool includes(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, InputIterator2 last2, Compare cmp)
{
    while (first1 != last1 && first2 != last2)
    {
        if (cmp(*first2, *first1))
            return false;
        else if (cmp(*first1, *first2))
            ++first1;
        else
        {
            ++first1;
            ++first2;
        }
    }
    return first2 == last2;
};

template <class ForwardIterator>
ForwardIterator max_element(ForwardIterator first, ForwardIterator last)
{
    if (first == last)
        return first;
    ForwardIterator result = first;
    while (++first != last)
        if (*result < *first)
            result = *first;
    return result;
}

template <class ForwardIterator, class Compare>
ForwardIterator max_element(ForwardIterator first, ForwardIterator last, Compare cmp)
{
    if (first == last)
        return first;
    ForwardIterator result = first;
    while (++first != last)
        if (cmp(*result, *first))
            result = *first;
    return result;
}

template <class ForwardIterator>
ForwardIterator min_element(ForwardIterator first, ForwardIterator last)
{
    if (first == last)
        return first;
    ForwardIterator result = first;
    while (++first != last)
        if (*first < *result)
            result = *first;
    return result;
}

template <class ForwardIterator, class Compare>
ForwardIterator min_element(ForwardIterator first, ForwardIterator last, Compare cmp)
{
    if (first == last)
        return first;
    ForwardIterator result = first;
    while (++first != last)
        if (cmp(*first, *result))
            result = *first;
    return result;
}

template <class InputIterator1, class InputIterator2, class OutputIterator>
OutputIterator merge(InputIterator1 first1, InputIterator1 last1,
                     InputIterator2 first2, InputIterator2 last2,
                     OutputIterator result)
{
    while (first1 != last1 && first2 != last2)
    {
        if (*first2 < *first1)
        {
            *result = *first2;
            ++first2;
        }
        else
        {
            *result = *first1;
            ++first1;
        }
        ++result;
    }
    return copy(first2, last2, copy(first1, last1, result));
}

template <class InputIterator1, class InputIterator2, class OutputIterator, class Compare>
OutputIterator merge(InputIterator1 first1, InputIterator1 last1,
                     InputIterator2 first2, InputIterator2 last2,
                     OutputIterator result, Compare cmp)
{
    while (first1 != last1 && first2 != last2)
    {
        if (cmp(*first2, *first1))
        {
            *result = *first2;
            ++first2;
        }
        else
        {
            *result = *first1;
            ++first1;
        }
        ++result;
    }
    return copy(first2, last2, copy(first1, last1, result));
}

template <class BidirectionalIterator, class Predicate>
BidirectionalIterator partition(BidirectionalIterator first, BidirectionalIterator last, Predicate pred)
{
    while (true)
    {
        while (true)
        {
            if (first == last)
                return first;
            else if (pred(*first))
                ++first;
            else
                break;
        }
        --last;
        while (true)
        {
            if (first == last)
                return first;
            else if (!pred(*last))
                --last;
            else
                break;
        }
        iter_swap(first, last);
        ++first;
    }
}

template <class InputIterator, class OutputIterator, class T>
OutputIterator remove_copy(InputIterator first, InputIterator last,
                           OutputIterator result, const T &value)
{
    for ( ; first != last; ++first)
    {
        if (*first != value)
        {
            *result = *first;
            ++result;
        }
    }
    return result;
}

template <class ForwardIterator, class T>
ForwardIterator remove(ForwardIterator first, ForwardIterator last, const T &value)
{
    first = find(first, last, value);
    ForwardIterator next = first;
    return first == last ? last : remove_copy(++next, last, first, value);
}

template <class InputIterator, class OutputIterator, class Predicate>
OutputIterator remove_copy_if(InputIterator first, InputIterator last,
                           OutputIterator result, Predicate pred)
{
    for ( ; first != last; ++first)
    {
        if (!pred(*first))
        {
            *result = *first;
            ++result;
        }
    }
    return result;
}

template <class ForwardIterator, class Predicate>
ForwardIterator remove_if(ForwardIterator first, ForwardIterator last, const Predicate pred)
{
    first = find_if(first, last, pred);
    ForwardIterator next = first;
    return first == last ? last : remove_copy_if(++next, last, first, pred);
}

template <class ForwardIterator, class T>
void replace(ForwardIterator first, ForwardIterator last, const T &old_vlaue, const T &new_value)
{
    for ( ; first != last; ++first)
        if (*first == old_vlaue)
            *first = new_value;
}

template <class ForwardIterator, class OutputIterator ,class T>
OutputIterator replace_copy(ForwardIterator first, ForwardIterator last, OutputIterator result, const T &old_vlaue, const T &new_value)
{
    for ( ; first != last; ++first, ++result)
        *result = (*first == old_vlaue) ? new_value : *first;
    return result;
}

template <class ForwardIterator, class Predicate, class T>
void replace_if(ForwardIterator first, ForwardIterator last, Predicate pred, const T &new_value)
{
    for ( ; first != last; ++first)
        if (pred(*first))
            *first = new_value;
}

template <class ForwardIterator, class OutputIterator, class Predicate, class T>
OutputIterator replace_copy(ForwardIterator first, ForwardIterator last, OutputIterator result, Predicate pred, const T &new_value)
{
    for ( ; first != last; ++first, ++result)
        *result = pred(*first) ? new_value : *first;
    return result;
}

template <class BidirectionalIterator>
void __reverse(BidirectionalIterator first, BidirectionalIterator last, bidirectional_iterator_tag)
{
    while (true)
    {
        if (first == last || first == --last)
            return;
        else
        {
            iter_swap(first, last);
            ++first;
        }
    }
}

template <class RandomAccessIterator>
void __reverse(RandomAccessIterator first, RandomAccessIterator last, random_access_iterator_tag)
{
    while (first < last)
    {
        iter_swap(first, --last);
        ++first;
    }
}

template <class BidirectionalIterator>
inline void reverse(BidirectionalIterator first, BidirectionalIterator last)
{
    __reverse(first, last, iterator_category(first));
}

template <class BidirectionalIterator, class OutputIterator>
OutputIterator reverse_copy(BidirectionalIterator first, BidirectionalIterator last, OutputIterator result)
{
    while (first != last)
    {
        --last;
        *result = *last;
        ++result;
    }
    return result;
}

template <class ForwardIterator, class Distance>
void __rotate(ForwardIterator first, ForwardIterator middle,
              ForwardIterator last, Distance*, forward_iterator_tag)
{
    for (ForwardIterator i = middle; ;)
    {
        iter_swap(first, i);
        ++first;
        ++i;

        // judge whether [first, middle) end first or [middle, last) end first
        if (first == middle)
        {
            if (i == last)
                return;
            middle = i;
        }
        else if (i == last)
            i = middle;
    }
}

template <class BidirectionalIterator, class Distance>
void __rotate(BidirectionalIterator first, BidirectionalIterator middle,
              BidirectionalIterator last, Distance*, bidirectional_iterator_tag)
{
    reverse(first, middle);
    reverse(middle, last);
    reverse(first, last);
}

template <class EuclideanRingElement>
EuclideanRingElement __gcd(EuclideanRingElement m, EuclideanRingElement n)
{
    while (n != 0)
    {
        EuclideanRingElement t = m % n;
        m = n;
        n = t;
    }
    return m;
}

template <class RandomAccessIterator, class Distance, class T>
void __rotate_cycle(RandomAccessIterator first, RandomAccessIterator last,
                    RandomAccessIterator initial, Distance shift, T*)
{
    T value = *initial;
    RandomAccessIterator ptr1 = initial;
    RandomAccessIterator ptr2 = ptr1 + shift;
    while (ptr2 != initial)
    {
        *ptr1 = *ptr2;
        ptr1 = ptr2;
        if (last - ptr2 > shift)
            ptr2 += shift;
        else
            ptr2 = first + (shift - (last - ptr2));
    }
    *ptr1 = value;
};

template <class RandomAccessIterator, class Distance>
void __rotate(RandomAccessIterator first, RandomAccessIterator middle,
              RandomAccessIterator last, Distance*, random_access_iterator_tag)
{
    Distance n = __gcd(last - first, middle - first);
    while (n--)
        __rotate_cycle(first, last, first + n, middle - first, value_type(first));
}

template <class ForwardIterator>
inline void rotate(ForwardIterator first, ForwardIterator middle, ForwardIterator last)
{
    if (first == middle || middle == last)
        return;
    __rotate(first, middle, last, distance_type(first), iterator_category(first));
}

template <class ForwardIterator, class OutputIterator>
OutputIterator rotate_copy(ForwardIterator first, ForwardIterator middle,
                           ForwardIterator last, OutputIterator result)
{
    return copy(first, middle, copy(middle, last, result));
};

template <class ForwardIterator, class Integer, class T>
ForwardIterator search_n(ForwardIterator first, ForwardIterator last,
                         Integer count, const T &value)
{
    if (count <= 0)
        return first;
    first = find(first, last, value);
    while (first != last)
    {
        Integer rest = count - 1;// should find count - 1 times in the rest sequence
        ForwardIterator i = first;
        ++i;
        while (rest > 0 && i != last && *i == value)
        {
            --rest;
            ++i;
        }
        if (rest == 0)
            return first;
        first = find(i, last, value);
    }
    return last;
};

template <class ForwardIterator, class Integer, class T, class BinaryPredicate>
ForwardIterator search_n(ForwardIterator first, ForwardIterator last,
                         Integer count, const T &value, BinaryPredicate binary_pred)
{
    if (count <= 0)
        return first;
    while (first != last)
    {
        if (binary_pred(*first, value))
            break;
        else
            ++first;
    }

    while (first != last)
    {
        Integer rest = count - 1;// should find count - 1 times in the rest sequence
        ForwardIterator i = first;
        ++i;
        while (rest > 0 && i != last && binary_pred(*i, value))
        {
            --rest;
            ++i;
        }
        if (rest == 0)
            return first;
        while (first != last)
        {
            if (binary_pred(*first, value))
                break;
            else
                ++first;
        }
    }
    return last;
};

template <class ForwardIterator1, class ForwardIterator2>
ForwardIterator2 swap_ranges(ForwardIterator1 first1, ForwardIterator1 last1,
                             ForwardIterator2 first2)
{
    for (; first1 != last1; ++first1, ++first2)
        iter_swap(first1, first2);
    return first2;
};

template <class InputIterator, class OutputIterator, class UnaryOperation>
OutputIterator transform(InputIterator first, InputIterator last, OutputIterator result, UnaryOperation op)
{
    for ( ; first != last; ++first, ++result)
        *result = op(*first);
    return result;
};

template <class InputIterator1, class InputIterator2, class OutputIterator, class BinaryOperation>
OutputIterator transform(InputIterator1 first1, InputIterator1 last, InputIterator2 first2,
                         OutputIterator result, BinaryOperation binary_op)
{
    for ( ; first1 != last; ++first1, ++first2, ++result)
        *result = binary_op(*first1, *first2);
    return result;
};

template <class InputIterator, class OutputIterator>
OutputIterator __unique_copy(InputIterator first, InputIterator last, OutputIterator result, forward_iterator_tag)
{
    *result = *first;
    while (++first != last)
    {
        if (*result != *first)
            *++result = *first;
    }
    return ++result;
};

template <class InputIterator, class OutputIterator, class T>
OutputIterator __unique_copy(InputIterator first, InputIterator last, OutputIterator result, T*)
{
    T value = *first;
    *result = value;
    while (++first != last)
    {
        if (*first != value)
        {
            value = *first;
            *++result  = value;
        }
    }
    return ++result;
};

template <class InputIterator, class OutputIterator>
inline OutputIterator __uique_copy(InputIterator first, InputIterator last, OutputIterator result, output_iterator_tag)
{
    return __unique_copy(first, last, result, value_type(first));
};

template <class InputIterator, class OutputIterator>
inline OutputIterator unique_copy(InputIterator first, InputIterator last, OutputIterator result)
{
    if (first == last)
        return result;
    return __unique_copy(first, last, result, iterator_category(first));
};

template <class ForwardIterator>
ForwardIterator unique(ForwardIterator first, ForwardIterator last)
{
    first = adjacent_find(first, last);
    return unique_copy(first, last, first);
}

template <class InputIterator, class OutputIterator, class BinaryPredicate>
OutputIterator __unique_copy(InputIterator first, InputIterator last, OutputIterator result,
                             BinaryPredicate binary_pred, forward_iterator_tag)
{
    *result = *first;
    while (++first != last)
    {
        if (!binary_pred(*result, *first))
            *++result = *first;
    }
    return ++result;
};

template <class InputIterator, class OutputIterator, class BinaryPredicate, class T>
OutputIterator __unique_copy(InputIterator first, InputIterator last, OutputIterator result,
                             BinaryPredicate binary_pred, T*)
{
    T value = *first;
    *result = value;
    while (++first != last)
    {
        if (!binary_pred(*first, value))
        {
            value = *first;
            *++result  = value;
        }
    }
    return ++result;
};

template <class InputIterator, class OutputIterator, class BinaryPredicate>
inline OutputIterator __uique_copy(InputIterator first, InputIterator last,
                                   OutputIterator result, BinaryPredicate binary_pred, output_iterator_tag)
{
    return __unique_copy(first, last, result, binary_pred, value_type(first));
};

template <class InputIterator, class OutputIterator, class BinaryPredicate>
inline OutputIterator unique_copy(InputIterator first, InputIterator last,
                                  OutputIterator result, BinaryPredicate binary_pred)
{
    if (first == last)
        return result;
    return __unique_copy(first, last, result, binary_pred, iterator_category(first));
};
template <class ForwardIterator, class BinaryPredicate>
ForwardIterator unique(ForwardIterator first, ForwardIterator last, BinaryPredicate binary_pred)
{
    first = adjacent_find(first, last, binary_pred);
    return unique_copy(first, last, first, binary_pred);
}

template <class ForwardIterator, class T, class Distance>
ForwardIterator __lower_bound(ForwardIterator first, ForwardIterator last,
                              const T &value, Distance*, forward_iterator_tag)
{
    Distance len = distance(first, last);
    Distance half;
    ForwardIterator middle;

    while (len > 0)
    {
        half = len >> 1;
        middle = first;
        advance(middle, half);
        if (*middle < value)
        {
            first = middle;
            ++first;
            len = len - half - 1;
        }
        else
            len = half;
    }
    return first;
};

template <class RandomAccessIterator, class T, class Distance>
RandomAccessIterator __lower_bound(RandomAccessIterator first, RandomAccessIterator last,
                                   const T &value, Distance*, random_access_iterator_tag)
{
    Distance len = distance(first, last);
    Distance half;
    RandomAccessIterator middle;

    while (len > 0)
    {
        half = len >> 1;
        middle = first + half;
        if (*middle < value)
        {
            first = middle + 1;
            len = len - half - 1;
        }
        else
            len = half;
    }
    return first;
};

template <class ForwardIterator, class T>
inline ForwardIterator lower_bound(ForwardIterator first, ForwardIterator last, const T &value)
{
    return __lower_bound(first, last, value, distance_type(first), iterator_category(first));
};

template <class ForwardIterator, class Compare, class T, class Distance>
ForwardIterator __lower_bound(ForwardIterator first, ForwardIterator last, Compare cmp,
                              const T &value, Distance*, forward_iterator_tag)
{
    Distance len = distance(first, last);
    Distance half;
    ForwardIterator middle;

    while (len > 0)
    {
        half = len >> 1;
        middle = first;
        advance(middle, half);
        if (cmp(*middle, value))
        {
            first = middle;
            ++first;
            len = len - half - 1;
        }
        else
            len = half;
    }
    return first;
};

template <class RandomAccessIterator, class Compare, class T, class Distance>
RandomAccessIterator __lower_bound(RandomAccessIterator first, RandomAccessIterator last, Compare cmp,
                                   const T &value, Distance*, random_access_iterator_tag)
{
    Distance len = distance(first, last);
    Distance half;
    RandomAccessIterator middle;

    while (len > 0)
    {
        half = len >> 1;
        middle = first + half;
        if (cmp(*middle, last))
        {
            first = middle + 1;
            len = len - half - 1;
        }
        else
            len = half;
    }
    return first;
};

template <class ForwardIterator, class T, class Compare>
inline ForwardIterator lower_bound(ForwardIterator first, ForwardIterator last,
                                   const T &value, Compare cmp)
{
    return __lower_bound(first, last, value, cmp, distance_type(first), iterator_category(first));
};

template <class ForwardIterator, class T, class Distance>
ForwardIterator __upper_bound(ForwardIterator first, ForwardIterator last,
                              const T &value, Distance*, forward_iterator_tag)
{
    Distance len = distance(first, last);
    Distance half;
    ForwardIterator middle;

    while (len > 0)
    {
        half = len >> 1;
        middle = first;
        advance(middle, half);
        if (value < *middle)
            len = half;
        else
        {
            first = middle;
            ++first;
            len = len - half - 1;
        }
    }
    return first;
};

template <class RandomAccessIterator, class T, class Distance>
RandomAccessIterator __upper_bound(RandomAccessIterator first, RandomAccessIterator last,
                                   const T &value, Distance*, random_access_iterator_tag)
{
    Distance len = distance(first, last);
    Distance half;
    RandomAccessIterator middle;

    while (len > 0)
    {
        half = len >> 1;
        middle = first + half;
        if (value < *middle)
            len = half;
        else
        {
            first = middle + 1;
            len = len - half - 1;
        }
    }
    return first;
};

template <class ForwardIterator, class T>
inline ForwardIterator upper_bound(ForwardIterator first, ForwardIterator last, const T &value)
{
    return __upper_bound(first, last, value, distance_type(first), iterator_category(first));
};

template <class ForwardIterator, class Compare, class T, class Distance>
ForwardIterator __upper_bound(ForwardIterator first, ForwardIterator last,
                              const T &value, Compare cmp, Distance*, forward_iterator_tag)
{
    Distance len = distance(first, last);
    Distance half;
    ForwardIterator middle;

    while (len > 0)
    {
        half = len >> 1;
        middle = first;
        advance(middle, half);
        if (cmp(value, *middle))
            len = half;
        else
        {
            first = middle;
            ++first;
            len = len - half - 1;
        }
    }
    return first;
};

template <class RandomAccessIterator, class Compare, class T, class Distance>
RandomAccessIterator __upper_bound(RandomAccessIterator first, RandomAccessIterator last,
                                   const T &value, Compare cmp, Distance*, random_access_iterator_tag)
{
    Distance len = distance(first, last);
    Distance half;
    RandomAccessIterator middle;

    while (len > 0)
    {
        half = len >> 1;
        middle = first + half;
        if (cmp(value, *middle))
            len = half;
        else
        {
            first = middle + 1;
            len = len - half - 1;
        }
    }
    return first;
};

template <class ForwardIterator, class T, class Compare>
inline ForwardIterator upper_bound(ForwardIterator first, ForwardIterator last,
                                   const T &value, Compare cmp)
{
    return __upper_bound(first, last, value, cmp, distance_type(first), iterator_category(first));
};

template <class ForwardIterator, class T>
bool binary_search(ForwardIterator first, ForwardIterator last, const T &value)
{
    ForwardIterator i = lower_bound(first, last, value); // now i == last or *i >= value
    return i != last && !(value < *i); // !(value < *i) <==> value >= *i logically
};

template <class ForwardIterator, class T, class Compare>
bool binary_search(ForwardIterator first, ForwardIterator last,
                   const T &value, Compare cmp)
{
    ForwardIterator i = lower_bound(first, last, value, cmp);
    return i != last && !cmp(value < *i);
};

template <class BidirectionalIterator>
bool next_permutation(BidirectionalIterator first, BidirectionalIterator last)
{
    if (first == last)
        return false;
    BidirectionalIterator i = first;
    ++i;
    if (i == last)
        return false;
    i = last;
    --i; // move i to the last element

    for (;;)
    {
        BidirectionalIterator ii = i;
        --i;
        if (*i < *ii)
        {
            BidirectionalIterator j = last;
            while (!(*i < *--j))
                ;
            iter_swap(i, j);
            reverse(ii, last);
            return true;
        }
        if (i == first)
        {
            reverse(first, last);
            return false;
        }
    }
}

template <class BidirectionalIterator, class Compare>
bool next_permutation(BidirectionalIterator first, BidirectionalIterator last, Compare cmp)
{
    if (first == last)
        return false;
    BidirectionalIterator i = first;
    ++i;
    if (i == last)
        return false;
    i = last;
    --i; // move i to the last element

    for (;;)
    {
        BidirectionalIterator ii = i;
        --i;
        if (cmp(*i, *ii))
        {
            BidirectionalIterator j = last;
            while (!cmp(*i, *--j))
                ;
            iter_swap(i, j);
            reverse(ii, last);
            return true;
        }
        if (i == first)
        {
            reverse(first, last);
            return false;
        }
    }
}



template <class BidirectionalIterator>
bool prev_permutation(BidirectionalIterator first, BidirectionalIterator last)
{
    if (first == last)
        return false;
    BidirectionalIterator i = first;
    ++i;
    if (i == last)
        return false;
    i = last;
    --i; // move i to the last element

    for (;;)
    {
        BidirectionalIterator ii = i;
        --i;
        if (*ii < *i)
        {
            BidirectionalIterator j = last;
            while (!(*--j < *i))
                ;
            iter_swap(i, j);
            reverse(ii, last);
            return true;
        }
        if (i == first)
        {
            reverse(first, last);
            return false;
        }
    }
}

template <class BidirectionalIterator, class Compare>
bool prev_permutation(BidirectionalIterator first, BidirectionalIterator last, Compare cmp)
{
    if (first == last)
        return false;
    BidirectionalIterator i = first;
    ++i;
    if (i == last)
        return false;
    i = last;
    --i; // move i to the last element

    for (;;)
    {
        BidirectionalIterator ii = i;
        --i;
        if (cmp(*ii, *i))
        {
            BidirectionalIterator j = last;
            while (!cmp(*--j, *i))
                ;
            iter_swap(i, j);
            reverse(ii, last);
            return true;
        }
        if (i == first)
        {
            reverse(first, last);
            return false;
        }
    }
}

template <class RandomAccessIterator, class Distance>
void __random_shuffle(RandomAccessIterator first, RandomAccessIterator last, Distance*)
{
    if (first == last)
        return;
    for (RandomAccessIterator i = first + 1; i != last; ++i)
    {
        iter_swap(i, first + Distance(::std::rand() % ((i - first) + 1)));
    }
};

template <class RandomAccessIterator>
inline void random_shuffle(RandomAccessIterator first, RandomAccessIterator last)
{
    __random_shuffle(first, last, distance_type(first));
}

template <class RandomAccessIterator, class RandomNumberGenerator>
inline void random_shuffle(RandomAccessIterator first, RandomAccessIterator last,
                           RandomNumberGenerator &rand) // pass by ref
{
    if (first == last)
        return;
    for (RandomAccessIterator i = first + 1; i != last; ++i)
        iter_swap(i, first + rand((i - first) + 1));
}


template <class RandomAccessIterator, class T>
void __partial_sort(RandomAccessIterator first, RandomAccessIterator middle,
                    RandomAccessIterator last, T*)
{
    make_heap(first, middle);
    for (RandomAccessIterator i = middle; i < last; ++i)
    {
        if (*i < *first)
            __pop_heap(first, middle, i, T(*i), distance_type(first));
        sort_heap(first, middle);
    }
};

template <class RandomAccessIterator>
inline void partial_sort(RandomAccessIterator first,
                         RandomAccessIterator middle,
                         RandomAccessIterator last)
{
    __partial_sort(first, middle, last, value_type(first));
}

template <class RandomAccessIterator, class Compare, class T>
void __partial_sort(RandomAccessIterator first, RandomAccessIterator middle,
                    RandomAccessIterator last, Compare cmp, T*)
{
    make_heap(first, middle);
    for (RandomAccessIterator i = middle; i < last; ++i)
    {
        if (cmp(*i, *first))
            __pop_heap(first, middle, i, cmp, T(*i), distance_type(first));
        sort_heap(first, middle, cmp);
    }
};

template <class RandomAccessIterator, class Compare>
inline void partial_sort(RandomAccessIterator first,
                         RandomAccessIterator middle,
                         RandomAccessIterator last, Compare cmp)
{
    __partial_sort(first, middle, last, cmp, value_type(first));
}

template <class RandomAccessIterator, class T>
inline void __unguarded_linear_insert(RandomAccessIterator last, T value)
{
    RandomAccessIterator next = last;
    --next;

    // don't have to check whether next underflow, since the first element must not larger
    // then the value, otherwise this function won't be called
    while (value < *next)
    {
        *last = *next;
        last = next;
        --next;
    }
    *last = value;
};

template <class RandomAccessIterator, class T>
inline void __linear_insert(RandomAccessIterator first, RandomAccessIterator last, T*)
{
    T value = *last;
    if (value < first)  // special case that all the elements larger than *last
    {
        copy_backward(first, last, last + 1);
        *first = value;
    }
    else
        __unguarded_linear_insert(last, value);
};

template <class RandomAccessIterator>
void __insertion_sort(RandomAccessIterator first, RandomAccessIterator last)
{
    if (first == last)
        return;
    for (RandomAccessIterator i = first + 1; i != last; ++i)
        __linear_insert(first, i, value_type(first));
}

template <class T>
inline const T& __median(const T &a, const T &b, const T &c)
{
    if (a < b)
    {
        if (b < c)
            return b;
        else if (a < c)
            return c;
        else
            return a;
    }
    else // a >= b
    {
        if (a < c)
            return a;
        else if (b < c)
            return c;
        else
            return b;
    }
}

template <class RandomAccessIterator, class T>
RandomAccessIterator __unguarded_partition(RandomAccessIterator first, RandomAccessIterator last, T pivot)
{
    while (true)
    {
        while (*first < *last)
            ++first;
        --last;
        while (pivot < *last)
            --last;
        if (!(first < last))
            return first;
        iter_swap(first, last);
        ++first;
    }
};

template <class Size>
inline Size __lg(Size n)
{
    Size k;
    for (k = 0; n > 1; n >> 1)
        ++k;
    return k; // return the index most significant 1 in the memory - 1
}

template <class RandomAccessIterator, class T, class Size>
void __introsort_loop(RandomAccessIterator first, RandomAccessIterator last, T*, Size depth_limit)
{
    while (last - first > __stl_threshold)
    {
        if (depth_limit == 0)
        {
            partial_sort(first, last, last);    // too much recursive partitions, using heap sort instead
            return;
        }

        --depth_limit;
        RandomAccessIterator  cut = __unguarded_partition(
                first, last, T(__median(
                        *first,
                        *(first + (last - first) / 2),
                        *(last - 1)
                ))
        );

        // recursive call self, to sort right half
        __introsort_loop(cut, last, value_type(first), depth_limit);
        last = cut;
    }
};

template <class RandomAccessIterator, class T>
void __unguarded_insertion_sort_aux(RandomAccessIterator first, RandomAccessIterator last, T*)
{
    for (RandomAccessIterator i = first; i != last; ++i)
        __unguarded_linear_insert(i, T(*i));
};

template <class RandomAccessIterator>
inline void __unguarded_insertion_sort(RandomAccessIterator first, RandomAccessIterator last)
{
    __unguarded_insertion_sort_aux(first, last, value_type(first));
}

template <class RandomAccessIterator>
void __final_insertion_sort(RandomAccessIterator first, RandomAccessIterator last)
{
    if (last - first > __stl_threshold)
    {
        __insertion_sort(first, first + __stl_threshold);
        __unguarded_insertion_sort(first + __stl_threshold, last);
    }
    else
    {
        // size of range <= threshold
        __insertion_sort(first, last);
    }
}

template <class RandomAccessIterator>
inline void sort(RandomAccessIterator first, RandomAccessIterator last)
{
    if (first == last)
        return;

    __introsort_loop(first, last, value_type(first), __lg(last - first) * 2);
    __final_insertion_sort(first, last);
}

template <class RandomAccessIterator, class T, class Distance>
pair<RandomAccessIterator, RandomAccessIterator>
__equal_range(RandomAccessIterator first, RandomAccessIterator last, const T &value,
              Distance*, random_access_iterator_tag)
{
    Distance len = last - first;
    Distance half;
    RandomAccessIterator middle, left, right;

    while (len > 0)
    {
        half = len >> 1;
        middle = first + half;
        if (*middle < value)
        {
            first = middle + 1;
            len = len - half - 1;
        }
        else if (value < *middle)
        {
            len = half;
        }
        else    // *middle == value
        {
            left = lower_bound(first, middle, value);
            right = upper_bound(++middle, first + len, value);
            return pair<RandomAccessIterator, RandomAccessIterator>(left, right);
        }
    }
    // can't find same value in the [first, last) range, return the iterator
    // that points the first element larger than value
    return pair<RandomAccessIterator, RandomAccessIterator>(first, first);
}

template <class ForwardIterator, class T, class Distance>
pair<ForwardIterator, ForwardIterator>
__equal_range(ForwardIterator first, ForwardIterator last, const T &value, Distance*, forward_iterator_tag)
{
    Distance len = distance(first, last);
    Distance half;
    ForwardIterator middle, left, right;

    while (len > 0)
    {
        half = len >> 1;
        middle = first;
        advance(middle, half);
        if (*middle < value)
        {
            first = middle;
            ++first;
            len = len - half - 1;
        }
        else if (value < *middle)
        {
            len = half;
        }
        else
        {
            left = lower_bound(first, middle, value);
            advance(first, len);
            right = upper_bound(++middle, first, value);
            return pair<ForwardIterator, ForwardIterator>(left, right);
        }
    }
    return pair<ForwardIterator, ForwardIterator>(first, first);
}

template <class ForwardIterator, class T>
inline pair<ForwardIterator, ForwardIterator>
equal_range(ForwardIterator first, ForwardIterator last, const T &value)
{
    return __equal_range(first, last, value, distance_type(first), iterator_category(first));
}

template <class BidirectionalIterator1, class BidirectionalIterator2, class Distance>
BidirectionalIterator1 __rotate_adaptive(BidirectionalIterator1 first,
                                         BidirectionalIterator1 middle,
                                         BidirectionalIterator1 last,
                                         Distance len1, Distance len2,
                                         BidirectionalIterator2 buffer,
                                         Distance buffer_size)
{
    BidirectionalIterator2 buffer_end;
    if (len1 > len2 && len2 <= buffer_size)
    {
        buffer_end = copy(middle, last, buffer);
        copy_backward(first, middle, last);
        return copy(buffer, buffer_end, first);
    }
    else if (len1 <= buffer_size)
    {
        buffer_end = copy(first, middle, buffer);
        copy(middle, last, first);
        return copy_backward(buffer, buffer_end, last);
    }
    else    // no enough space
    {
        rotate(first, middle, last);
        advance(first, len2);
        return first;
    }
};


template <class BidirectionalIterator, class Distance, class Pointer>
void __merge_adaptive(BidirectionalIterator first, BidirectionalIterator middle,
                      BidirectionalIterator last, Distance len1, Distance len2,
                      Pointer buffer, Distance buffer_size)
{
    if (len1 <= len2 && len1 <= buffer_size)
    {
        Pointer end_buffer = copy(first, middle, buffer);
        merge(buffer, end_buffer, middle, last, first);
    }
    else if (len2 <= buffer_size)
    {
        Pointer end_buffer = copy(middle, last, buffer);
        merge_backward(first, middle, buffer, end_buffer, last);
    }
    else    // buffer is too small for both ranges
    {
        BidirectionalIterator first_cut = first;
        BidirectionalIterator second_cut = middle;
        Distance len1_1 = 0;
        Distance len2_2 = 0;

        // process the longer range, cut it into two parts
        if (len1 > len2)
        {
            len1_1 = len1 / 2;
            advance(first_cut, len1_1); // first_cut points the mid of first range
            second_cut = lower_bound(middle, last, *first_cut);
            len2_2 = distance(middle, second_cut);
        }
        else
        {
            len2_2 = len2 / 2;
            advance(second_cut, len2_2);
            first_cut = upper_bound(first, middle, *second_cut);
            len1_1 = distance(first, first_cut);
        }

        BidirectionalIterator new_middle =
                __rotate_adaptive(first_cut, middle, second_cut, len1 - len1_1, len2_2, buffer, buffer_size);

        __merge_adaptive(first, first_cut, new_middle, len1_1, len2_2, buffer, buffer_size);
        __merge_adaptive(new_middle, second_cut, last, len1 - len1_1, len2 - len2_2, buffer, buffer_size);
    }
};

template <class BidirectionalIterator, class T, class Distance>
inline void __inplace_merge_aux(BidirectionalIterator first, BidirectionalIterator middle,
                                BidirectionalIterator last, T*, Distance*)
{
    Distance len1 = distance(first, middle);
    Distance len2 = distance(middle, last);

    temporary_buffer<BidirectionalIterator, T> buf(first, last);
    if (buf.begin() == 0) // alloc fail
        merge_without_buffer(first, middle, last, len1, len2);
    else
        __merge_adaptive(first, middle, last, len1, len2, buf.begin(), Distance(buf.size()));
};

template <class BidirectionalIterator>
inline void inplace_merge(BidirectionalIterator first,
                          BidirectionalIterator middle,
                          BidirectionalIterator last)
{

    if (first == middle || middle == last)
        return;
    __inplace_merge_aux(first, middle, last, value_type(first), distance_type(first));
}

template <class RandomAccessIterator, class T>
void __nth_element(RandomAccessIterator first,
                   RandomAccessIterator nth,
                   RandomAccessIterator last, T*)
{
    while (last - first > 3)
    {
        RandomAccessIterator cut = __unguarded_partition(first, last, T(__median(*first,
                                                                                 *(first + (last - first)/2),
                                                                                 *(last - 1)
        )));
        if (cut < nth)
            first = cut;
        else
            last = cut;
    }
    __insertion_sort(first, last);
};

template <class RandomAccessIterator>
inline void nth_element(RandomAccessIterator first, RandomAccessIterator nth, RandomAccessIterator last)
{
    __nth_element(first, nth, last, value_type(first));
}

}