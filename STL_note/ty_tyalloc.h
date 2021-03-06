#pragma once
#ifndef _TY_TYALLOC_H
#define _TY_TYALLOC_H

#include <new>      // for placement new
#include <cstddef>  // for size_t, ptrdiff_t
#include <cstdlib>  // for exit()
#include <climits>  // for UINT_MAX
#include <iostream> // for cerr

namespace ty
{
template<typename T>
inline T* allocate(ptrdiff_t size, T*)
{
    ::std::set_new_handler(0);
    T *tmp = (T*)(::operator new((size_t)(size * sizeof(T))));
    if (!tmp)
    {
        std::cerr << "out of memory" << std::endl;
        exit(1);
    }
    return tmp;
}

template<typename T>
inline void deallocate(T *buffer)
{
    ::operator delete(buffer);
}

template <typename T>
class allocator
{
public:
    typedef T           value_type;
    typedef T*          pointer;
    typedef const T*    const_pointer;
    typedef T&          reference;
    typedef const T&    const_reference;
    typedef size_t      size_type;
    typedef ptrdiff_t   difference_type;

    // rebind allocator of type U
    template <typename U>
    struct rebind
    {
        typedef allocator<U> other;
    };

    // hint used for locality
    pointer allocate(size_type n, const void *hint = 0)
    {
        return ::ty::allocate(static_cast<difference_type>(n),
                          static_cast<pointer>(0));
    }

    void deallocate(pointer ptr)
    {
        ::ty::deallocate(ptr);
    }

    template <typename U>
    void construct(pointer ptr, const U &value)
    {
        ::ty::construct(ptr, value);
    }

    void destroy(pointer ptr)
    {
        ::ty::destroy(ptr);
    }

    pointer address(reference x)
    {
        return static_cast<pointer>(&x);
    }

    const_pointer const_address(const_reference x)
    {
        return static_cast<const_pointer>(&x);
    }

    const_pointer const_address(reference x)
    {
        return static_cast<const_pointer>(&x);
    }

    size_type max_size()const
    {
        return static_cast<size_type>(UINT_MAX / sizeof(T));
    }

};
}// namespace ty

#endif // !_TY_TYALLOC_H