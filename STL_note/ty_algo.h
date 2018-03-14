#pragma once

#include "ty_algobase.h"
#include "ty_heap.h"

namespace ty
{
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
















}