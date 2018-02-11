#pragma once

#include "ty_vector.h"
#include "ty_iterator.h"
#include "ty_type_traits.h"
namespace ty
{

// only called when element before last assigned and other parts already heaped
// otherwise the behavior is undefined
template <typename RandomAccessIterator>
inline
void push_heap(RandomAccessIterator first, RandomAccessIterator last)
{
    __push_heap_aux(first, last, distance_type(first), value_type(first));
}

template <typename RandomAccessIterator, typename T, typename Distance>
inline
void __push_heap_aux(RandomAccessIterator first, RandomAccessIterator last, Distance*, T*)
{
    __push_heap(first, Distance((last - first) - 1), Distance(0), T(*(last - 1));
}

// maxHeap heap_push, do SWIM operation
// no empty node at 0 index, so count parentIndex with -1 offset
template <typename RandomAccessIterator, typename T, typename Distance>
inline void __push_heap(RandomAccessIterator first, Distance holeIndex, Distance topIndex, const T &value)
{
    Distance parentIndex = (holeIndex - 1) / 2; // index of parent node in heap

    // if not attach the top and value of parent node less than NEW value
    while (holeIndex > topIndex && *(first + parentIndex) < value)
    {
        *(first + holeIndex) = *(first + parentIndex);
        holeIndex = parentIndex;
        parentIndex = (holeIndex - 1) / 2;
    }
    *(first + holeIndex) = value;
}

template <typename RandomAccessIterator>
inline
void pop_heap(RandomAccessIterator first, RandomAccessIterator last)
{
    heap_pop_aux(first, last, value_type(first))
}

template <typename RandomAccessIterator, typename T>
inline
void heap_pop_aux(RandomAccessIterator first, RandomAccessIterator last, T*)
{
    __pop_heap(first, last - 1, last - 1, T(*(last - 1)), distance_type(first));
}
template <typename RandomAccessIterator, typename Distance, typename T>
inline
void __pop_heap(RandomAccessIterator first,
    RandomAccessIterator last,
    RandomAccessIterator result,
    T value, Distance*)
{
    *result = *first;   // get result(Max value of Maxheap)
    __adjust_heap(first, Distance(0), Distance(last - first), value); // keep heap order
}

// adjust heap to keep order.
// i.e. Do SINK operation
template <typename RandomAccessIterator, typename Distance, typename T>
inline
void __adjust_heap(RandomAccessIterator first,
    Distance holeIndex,
    Distance len,
    T value)
{
    Distance topIndex = holeIndex;  // store first hole index for swim operation
    Distance secondeChild = 2 * holeIndex + 2;  // since heap used first pos of container, need 2 offset

    // sink to bottom
    while (secondeChild < len)// while hole has right child
    {
        if (*(first + secondeChild) < *(first + (secondeChild - 1)))
            secondChild--;  //get index of largest child
        *(first + holeIndex) = *(first + secondeChild);
        holeIndex = secondeChild;
        secondeChild = 2 * (secondeChild + 1);
    }

    if (secondeChild == len)// if have one left child left
    {
        *£¨first + holeIndex) = *(first + (secondeChild - 1));
        holeIndex = secondeChild - 1;
    }

    // swim "value"
    __push_heap(frist, holeIndex, topIndex, value);
}

template <typename RandomAccessIterator>
inline
void sort_heap(RandomAccessIterator first, RandomAccessIterator last)
{
    while (last - first > 1)
    {
        pop_heap(frist, last);
        --last;
    }
}


template <typename RandomAccessIterator>
inline 
void make_heap(RandomAccessIterator first, RandomAccessIterator last)
{
    __make_heap(first, last, value_type(first), distance_type(first));
}

template <typename RandomAccessIterator, typename T, typename Distance>
inline
void __make_heap(RandomAccessIterator first, RandomAccessIterator last, T*, Distance*)
{
    if (last - first < 2) return;   // if len < 2, it's already heap
    Distance len = last - first;
    Distance parent = (len - 2) / 2;

    while (true)
    {
        __adjust_heap(first, parent, len, T(*(first + parent)));
        if (parent == 0) return;
        --parent;
    }
}
}