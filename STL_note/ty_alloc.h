#pragma once

#ifndef _TY_ALLOC_H
#define _TY_ALLOC_H

namespace ty {

// if you want to change the config, change 0 to 1
#if 0
#   include <new>
#   define __THROW_BAD_ALLOC throw std::bad_alloc
#elif !defined(__THROW_BAD_ALLOC)
#   include <iostream>
#   define __THROW_BAD_ALLOC std::cerr << "out of memory" << std::endl; exit(1)
#endif

// inst makes nothing
template <int inst>
class __malloc_alloc_template
{
private:
    // oom : out of memory
    static void* oom_malloc(size_t n);
    static void* oom_realloc(void *, size_t);
    static void(*__malloc_alloc_oom_handler)();

public:
    static void* allocate(size_t n)
    {
        auto result = malloc(n);
        // if memorty not enough, call oom_malloc()
        if(result == nullptr)  result = oom_malloc(n);
        return result;
    }

    static void deallocate(void *ptr, size_t /* n */)
    {
        free(ptr);
    }

    static void *reallocate(void *p, size_t /* old_sz*/, size_t new_sz)
    {
        void * result = realloc(p, new_sz);
        if (0 == result)
            result = oom_realloc(p, new_sz);
        return result;
    }

    // monitor set_new_handler
    static void(*set_malloc_handler(void(*f)())) ()
    {
        void(*old) () = __malloc_alloc_oom_handler; // store the old handler

        __malloc_alloc_oom_handler = f;
        return (old);
    }
};

// malloc_alloc init out-of-memory handling
template <int inst>
void(*__malloc_alloc_template<inst>::__malloc_alloc_oom_handler)() = 0;

template <int inst>
void* __malloc_alloc_template<inst>::oom_malloc(size_t n)
{
    void(*my_malloc_handler)();
    void *result;

    for (;;) // always try
    {
        my_malloc_handler = __malloc_alloc_oom_handler;
        if (0 == my_malloc_handler)
        {
            __THROW_BAD_ALLOC; // if handler is unset, throw bad_alloc
        }
        (*my_malloc_handler)(); // call handler
        result = malloc(n);
        if (result)
            return result;
    }
}

template <int inst>
void * __malloc_alloc_template<inst>::oom_realloc(void *p, size_t n)
{
    void(*my_handler)();
    void *result;

    for (;;)
    {
        my_handler = __malloc_alloc_oom_handler;
        if (0 == my_handler)
        {
            __THROW_BAD_ALLOC; // if handler is unset, throw bad_alloc
        }
        (*my_handler)();
        result = realloc(p, n); // try realloc
        if (result)
            return result;
    }
}

typedef __malloc_alloc_template<0> malloc_alloc;

enum
{
    __ALIGN = 8     // 小型区块的上调边界
};
enum
{
    __MAX_BYTES = 128 // max size of small block
};
enum
{
    __NFREELISTS = __MAX_BYTES / __ALIGN  // the number of free-lists
};

// threads value used in multi-thread situation
// inst will not be used
template <bool threads, int inst>
class __default_alloc_template
{
private:
    // increase the value of bytes to the multiple of __ALIGN
    static size_t ROUND_UP(size_t bytes)
    {
        return (((bytes)+__ALIGN - 1) & ~__ALIGN);
    }

    union obj
    {
        // this union assure its right behavior in old enviroment
        union obj *free_list_link;
        char client_data[1];
    };

    // __NFREELISTS free lists
    static obj * volatile free_list[__NFREELISTS];

    // choose the appropriate free-list according to the size of byte, index start from 0
    static size_t FREELIST_INDEX(size_t bytes)
    {
        return (((bytes)+__ALIGN - 1) / __ALIGN - 1);
    }

    // return an size n object, may add other size n blocks to free list
    // n must be multiple of __ALIGN
    static void * refill(size_t n);

    // allocate a big chunk of memory to store n blocks of size n
    // if it is not convenient to allocate n blocks, nobjs may decrease
    static char *chunk_alloc(size_t size, int &nobjs);

    // Chunk allocation state
    static char *start_free;    // start pos of memory pool
    static char *end_free;      // end pos of memory pool
    static size_t heap_size;

public:
    static void * allocate(size_t n);
    static void deallocate(void *p, size_t n);

    static void * reallocate(void *p, size_t old_sz, size_t new_sz) 
    {
        return nullptr; 
    }
};

// Start: Init static field of __dafault_alloc_template
template <bool threads, int inst>
char *__default_alloc_template<threads, inst>::start_free = 0; // init the start of memory pool

template <bool threads, int inst>
char *__default_alloc_template<threads, inst>::end_free = 0; // init the end of memory poo;

template <bool threads, int inst>
size_t __default_alloc_template<threads, inst>::heap_size = 0; // init heap-size

template <bool threads, int inst>
typename __default_alloc_template<threads, inst>::obj *volatile 
__default_alloc_template<threads, inst>::free_list[__NFREELISTS] ={ 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
};
// End: Init static field of __dafault_alloc_template

template <bool threads, int inst>
void *__default_alloc_template<threads, inst>::allocate(size_t n)
{
    obj * result;
    obj * volatile * my_free_list;
    // if the size too large, call melloc_alloc
    if (n > __MAX_BYTES)
        return malloc_alloc::allocate(n);
    // get the appropriate free list of size n
    my_free_list = free_list + FREELIST_INDEX(n);
    result = *my_free_list;
    if (0 == result)
    {
        void *rtn = refill(n);
        return rtn;
    }
    *my_free_list = result->free_list_link;
    return result;
}

template <bool threads, int inst>
void __default_alloc_template<threads, inst>::deallocate(void *p, size_t n)
{
    // do nothing if p is nullptr
    if (0 == p)
        return;

    // if the size too large, call malloc_alloc::deallocate
    if (n > __MAX_BYTES)
    {
        malloc_alloc::deallocate(p, n);
        return;
    }
    // find corresponding free list
    obj * volatile *my_free_list = free_list + FREELIST_INDEX(n);
    // adjust the free list, collect the memory back
    static_cast<obj*>(p)->free_list_link = *my_free_list;
    *my_free_list = static_cast<obj*>(p);
}

// return an size n object, may add other size n blocks to free list
// n must be multiple of __ALIGN
template <bool threads, int inst>
void *__default_alloc_template<threads, inst>::refill(size_t n)
{
    int nobjs = 20;

    // try to get nobjs blocks of size n, may change nobjs if no enough memory
    char *chunk = chunk_alloc(n, nobjs);
    if (1 == nobjs)
    {
        return (chunk);// return if only return one block
    }

    obj *result = (obj*)(chunk);// return value

    // construct a new free_list on the chunk memory
    obj *volatile *my_free_list = free_list + FREELIST_INDEX(n);// get my free list
    obj *current_block;
    obj *next_block = (obj*)(chunk + n);
    *my_free_list = next_block;
    for (int i = 1; ; i++)
    {
        current_block = next_block;
        next_block = (obj*)((char*)(next_block) + n);
        if (nobjs - 1 == i)
        {
            current_block->free_list_link = nullptr;// construction complete
            break;
        }
        else
        {
            current_block->free_list_link = next_block;
        }
    }
    return (result);
}

template <bool threads, int inst>
char *__default_alloc_template<threads, inst>::chunk_alloc(size_t size, int &nobjs)
{
    obj *result;
    size_t total_bytes = size * nobjs;// size of memory to get
    size_t rest_size = end_free - start_free;// rest contend of memory pool

    if (rest_size >= total_bytes)   // memory enough
    {
        result = (obj*)start_free;
        start_free += total_bytes;
        return (char*)(result);
    }
    else if (rest_size >= size)     // enough for at least one block of size size
    {
        nobjs = rest_size / size;// true assign numbes of blocks
        total_bytes = size * nobjs;
        result = (obj*)start_free;
        start_free += total_bytes;
        return (char*)(result);
    }
    else                            // memory too less, try to get memory from free_list
    {
        // if there are still some small block of memory, assign it the approrirate free list
        if (rest_size > 0)
        {
            // first, find appropriate free list
            obj *volatile *my_free_list = free_list + FREELIST_INDEX(rest_size);
               
            // link the rest memory of the memory pool the my_free_list
            ((obj*)start_free)->free_list_link = *my_free_list;
            *my_free_list = (obj*)(start_free);
        }

        // double of target_size puls a increasing random size memory
        size_t bytes_to_get = 2 * total_bytes + ROUND_UP(heap_size >> 4);

        // config heap space to supply the memory pool
        start_free = static_cast<char*>(malloc(bytes_to_get));

        // heap space not enough
        if (0 == start_free)
        {
            int block_size;
            obj *volatile *my_free_list;
            obj *pointer_to_block;

            // 开始搜寻适当的free list
            // 所谓适当是指 “尚有未用区块，且区块够大”的 free list
            for (block_size = size; block_size <= __MAX_BYTES; block_size += __ALIGN)
            {
                my_free_list = free_list + FREELIST_INDEX(block_size);
                pointer_to_block = *my_free_list;

                if (0 != my_free_list) // if this list is not empty
                {
                    // adjust this list to release the unused block
                    *my_free_list = pointer_to_block->free_list_link;

                    // add this block to memory pool
                    start_free = (char*)(pointer_to_block);
                    end_free = start_free + block_size;

                    // call self to adjust nobjs
                    return (chunk_alloc(size, nobjs));
                }
            }

            // if no appropriate free list (nowhere to find enough memory)
            // try to get help from out-of-memory function
            end_free = 0;
            start_free = static_cast<char*>(malloc_alloc::allocate(bytes_to_get));
        }

        heap_size += bytes_to_get;
        end_free = start_free + bytes_to_get;
        // self call, to adjust nobjs
        return (chunk_alloc(size, nobjs));
    }
}

// alloc and dealloc by number of elements, while malloc_alloc and default_alloc_template manage
// memory by size of bytes
template <typename T, typename Alloc>
class simple_alloc
{
public:
    static T* allocate(size_t n)
    {
        return 0 == n ? 0 : (T*)Alloc::allocate(n * sizeof(T));
    }
    // allocate one obj
    static T* allocate(void)
    {
        return (T*)Alloc::allocate(sizeof(T));
    }
    static void deallocate(T *p, size_t n)
    {
        if (0 != n)
            Alloc::deallocate(p, n * sizeof(T));
    }
    static void deallocate(T *p)
    {
        Alloc::deallocate(p, sizeof(T));
    }
};
}// namespace ty

#endif // !_TY_ALLOC_H
