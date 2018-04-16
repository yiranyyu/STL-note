//
// Created by yirany on 2018/3/31.
//

#ifndef THE_ANNOTATED_STL_SOURCES_TY_BUFFER_H
#define THE_ANNOTATED_STL_SOURCES_TY_BUFFER_H

#include "ty_alloc.h"
#include "ty_iterator.h"

namespace ty
{
    template <class Iterator, class T, class Alloc = alloc>
    class temporary_buffer
    {
    private:
        using value_type = T;
        using data_allocator = simple_alloc<value_type, Alloc>;
        using iterator = T*;
        using difference_type = ptrdiff_t;
        using iterator_category = random_access_iterator_tag;
        using size_type = size_t;
    public:
        temporary_buffer(Iterator first, Iterator last)
        {
            typename iterator_traits<Iterator>::difference_type size
                    = distance(first, last);
            start = data_allocator::allocate(size);
        }
        iterator begin()
        {
            return start;
        }
    private:
        iterator start;
    };
}



#endif //THE_ANNOTATED_STL_SOURCES_TY_BUFFER_H
