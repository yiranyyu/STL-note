#pragma once

#ifndef _TY_ITERATOR_H
#define _TY_ITERATOR_H

#include "ty_types.h"
#include "ty_function.h"
#include <iostream>
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
    typedef Reference       reference;
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

template <class Container>
class back_insert_iterator
{
protected:
    Container *container;
public:
    using iterator_category = output_iterator_tag;
    using value_type        = void;
    using difference_type   = void;
    using pointer           = void;
    using reference         = void;
    using container_type    = Container;

    explicit back_insert_iterator(Container &x)
            : container(&x)
    {}

    back_insert_iterator&
    operator=(const typename Container::value_type &value)
    {
        container->push_back(value);
        return *this;
    }

    // these functions just return *this and do nothing
    back_insert_iterator& operator*()
    {
        return *this;
    }

    back_insert_iterator& operator++()
    {
        return *this;
    }

    back_insert_iterator& operator++(int)
    {
        return *this;
    }
};

template <class Container>
inline back_insert_iterator<Container> back_inserter(Container &x)
{
    return back_insert_iterator<Container>(x);
}

template <class Container>
class front_insert_iterator
{
protected:
    Container *container;
public:
    using iterator_category = output_iterator_tag;
    using value_type        = void;
    using difference_type   = void;
    using pointer           = void;
    using reference         = void;
    using container_type    = Container;

    explicit front_insert_iterator(Container &x)
            : container(&x)
    {}

    front_insert_iterator&
    operator=(const typename Container::value_type& value)
    {
        container->push_front(value);
        return *this;
    }

    front_insert_iterator& operator*()
    {
        return *this;
    }

    front_insert_iterator& operator++()
    {
        return *this;
    }

    front_insert_iterator& operator++(int)
    {
        return *this;
    }
};

template <class Container>
inline front_insert_iterator<Container> front_inserter(Container &x)
{
    return front_insert_iterator<Container>(x);
}

template <class Container>
class insert_iterator
{
protected:
    Container *container;
    typename Container::iterator iter;
public:
    using iterator_category = output_iterator_tag;
    using value_type        = void;
    using difference_type   = void;
    using pointer           = void;
    using reference         = void;
    using container_type    = Container;

    insert_iterator(Container &x, typename Container::iterator i)
            : container(&x), iter(i)
    {}

    insert_iterator&
    operator=(const typename container_type::value_type &value)
    {
        iter = container->insert(iter, value);
        ++iter; // make the iterator always points the same position
        return *this;
    }

    insert_iterator& operator*()
    {
        return *this;
    }

    insert_iterator& operator++()
    {
        return *this;
    }

    insert_iterator& operator++(int)
    {
        return *this;
    }
};

template <class Container, class Iterator>
inline insert_iterator<Container> inserter(Container &x, Iterator i)
{
    using iter = typename Container::iterator;
    return insert_iterator<Container>(x, iter(i));
};


template <class Iterator>
class reverse_iterator
{
protected:
    Iterator current;
public:
    using iterator_category = typename iterator_traits<Iterator>::iterator_category;
    using value_type = typename iterator_traits<Iterator>::value_type;
    using difference_type = typename iterator_traits<Iterator>::difference_type;
    using pointer = typename iterator_traits<Iterator>::pointer;
    using reference = typename iterator_traits<Iterator>::reference;

    using iterator_type = Iterator;
    using self = reverse_iterator<Iterator>;

public:
    reverse_iterator()
    {
        // empty
    }

    explicit reverse_iterator(iterator_type x)
        :current(x)
    {}

    reverse_iterator(const self &x)
        :current(x.current)
    {}

    iterator_type base() const {return current;}

    reference operator*() const
    {
        Iterator tmp = current;
        return *--tmp;
    }

    pointer operator->() const
    {
        return &(operator*());
    }

    self& operator++()
    {
        --current;
        return *this;
    }

    self operator++(int)
    {
        self tmp = *this;
        --current;
        return tmp;
    }

    self& operator--()
    {
        ++current;
        return *this;
    }

    self operator--(int)
    {
        self tmp = *this;
        ++current;
        return tmp;
    }

    self operator+(difference_type n) const
    {
        return self(current - n);
    }

    self& operator+=(difference_type n)
    {
        current -= n;
        return *this;
    }

    self operator-(difference_type n) const

    {
        return self(current + n);
    }

    self& operator-=(difference_type n)
    {
        current += n;
        return *this;
    }

    reference operator[] (difference_type n) const
    {
        return *(*this + n);
    }
};

template <class T, class Distance>
class istream_iterator;

// equals if bound with same stream and all marked as end of stream         <---- self designed
template <class T, class Distance>
bool operator==(const istream_iterator<T, Distance> &x,
                const istream_iterator<T, Distance> &y)
{
    return x.stream == y.stream;
};

template <class T, class Distance = ptrdiff_t>
class istream_iterator
{
    friend bool operator==<T, Distance>(const istream_iterator<T, Distance> &x,
                             const istream_iterator<T, Distance> &y);

public:
    using iterator_category = input_iterator_tag;
    using value_type = const T;
    using reference = T&;
    using pointer = const T*;
    using difference_type = Distance;
    using self = istream_iterator<T, Distance>;

protected:
    using istream = typename ::std::istream;
    istream *stream;
    T value;

public:
    istream_iterator()
            :stream(nullptr)
    {}

    explicit istream_iterator(istream &in)
            :stream(&in)
    {
        read();
    }

    void read()
    {
        if (stream != 0 && !(*stream >> value))
        {
            stream  = 0;
        }
    }

    reference operator*() const
    {
        return value;
    }

    pointer operator->() const
    {
        return &(operator*());
    }

    self& operator++()
    {
        read();
        return *this;
    }

    self operator++(int)
    {
        self temp = *this;
        read();
        return temp;
    }

};


template <class T>
class ostream_iterator
{
public:
    using iterator_category = output_iterator_tag;
    using value_type = void;
    using pointer = void;
    using reference = void;
    using difference_type = void;
    using self = ostream_iterator<T>;

protected:
    using ostream = typename ::std::ostream;
    ostream *stream;
    const char *string;

public:
    explicit ostream_iterator(ostream &os)
        :stream(&os)
    {}

    ostream_iterator(ostream &os, const char *str)
        :stream(&os), string(str)
    {}

    self& operator=(const T value)
    {
        *stream << value;
        if (string)
            *stream << string;
        return *this;
    }

    self& operator++()
    {
        return *this;
    }

    self& operator++(int)
    {
        return *this;
    }

    self operator*()
    {
        return *this;
    }
};


template <class Predicate>
class unary_negate : public unary_function<typename Predicate::argument_type, bool>
{
protected:
    Predicate pred;
public:
    explicit unary_negate(const Predicate _pred)
        :pred(_pred)
    {}

    bool operator()(const typename Predicate::argument_type & arg) const
    {
        return !(pred(arg));
    }
};


template <class Predicate>
inline unary_negate<Predicate> not1(const Predicate &pred)
{
    return unary_negate<Predicate>(pred);
}

template <class Predicate>
class binary_negate: public binary_function<typename Predicate::first_argument_type,
                                            typename Predicate::second_argument_type, bool>
{
protected:
    Predicate pred;
public:
    explicit binary_negate(const Predicate &_pred)
        :pred(_pred)
    {}

    bool operator()(const typename Predicate::first_argument_type &lhs,
                    const typename Predicate::first_argument_type &rhs)const
    {
        return !(pred(lhs, rhs));
    }
};


template <class Predicate>
inline binary_negate<Predicate> not2(const Predicate &pred)
{
    return binary_negate<Predicate>(pred);
}

template <class Operation>
class binder1st: public unary_function<typename Operation::second_argument_type, typename Operation::result_type>
{
protected:
    Operation op;
    typename Operation::first_argument_type first_value;
public:
    binder1st(const Operation &_op, const typename Operation::first_argument_type &first)
        :op(_op), first_value(first)
    {}

    typename Operation::result_type
    operator()(const typename Operation::second_argument_type &arg) const
    {
        return (op(first_value, arg));
    }
};

template <class Operation, class T>
inline
binder1st<Operation> bind1st(const Operation &op, const T &x)
{
    using first_arg_type = typename Operation::first_argument_type;
    return binder1st<Operation>(op, first_arg_type(x));             // check and assure type
};


template <class Operation>
class binder2nd: public unary_function<typename Operation::first_argument_type, typename Operation::result_type>
{
protected:
    Operation op;
    typename Operation::second_argument_type second_value;
public:
    binder2nd(const Operation &_op, const typename Operation::second_argument_type &value)
        :op(_op), second_value(value)
    {}

    typename Operation::result_type
    operator()(const typename Operation::first_argument_type &arg) const
    {
        return (op(arg, second_value));
    }
};

template <class Operation, class T>
inline
binder2nd<Operation> bind2nd(const Operation &op, const T &x)
{
    using second_arg_type = typename Operation::second_argument_type;
    return binder2nd<Operation>(op, second_arg_type(x));
};

// for procedure f and g, this class operate synthesized f·g
template <class Operation1, class Operation2>
class unary_compose: public unary_function<typename Operation2::argument_type,
                                           typename Operation1::result_type>
{
protected:
    Operation1 op1;
    Operation2 op2;
public:
    unary_compose(const Operation1 &_op1, const Operation2 &_op2)
        :op1(_op1), op2(_op2)
    {}

    typename Operation1::result_type
    operator()(const typename Operation2::argument_type &arg) const
    {
        return (op1(op2(arg)));
    }
};


template <class Operation1, class Operation2>
inline auto
compose1(const Operation1 &op1, const Operation2 &op2)
{
        return unary_compose<Operation1, Operation2>(op1, op2);
};

// compose procedure f, g, h to f(g(), h()), g and h are unary functor
template <class Operation1, class Operation2, class Operation3>
class binary_compose: public unary_function<typename Operation2::argument_type,
                                            typename Operation1::result_type>
{
protected:
    Operation1 op1;
    Operation2 op2;
    Operation3 op3;

public:
    binary_compose(const Operation1 &_op1, const Operation2 &_op2, const Operation3 &_op3)
        :op1(_op1), op2(_op2), op3(_op3)
    {}

    typename Operation1::result_type
    operator()(const typename Operation2::argument_type &arg) const
    {
        return (op1(op2(arg), op3(arg)));
    }
};

template <class Operation1, class Operation2, class Operation3>
inline auto
compose2(const Operation1 &op1, const Operation2 &op2, const Operation3 &op3)
{
    return binary_compose<Operation1, Operation2, Operation3>(op1, op2, op3);
};

template <class Arg, class Result>
class pointer_to_unary_function: public unary_function<Arg, Result>
{
protected:
    Result (*ptr)(Arg);

public:
    pointer_to_unary_function()
        :ptr(nullptr)
    {}

    explicit pointer_to_unary_function(decltype(ptr) _ptr)
        :ptr(_ptr)
    {}

    Result operator()(Arg arg) const                            // using parameter_type directly (i.e., no cv qualifier)
    {
        return ptr(arg);
    }
};

template <class Arg, class Result>
inline auto
ptr_fun(Result (*x) (Arg))
{

};


template <class Arg1, class Arg2, class Result>
class pointer_to_binary_function: public binary_function<Arg1, Arg2, Result>
{
protected:
    Result (*ptr)(Arg1, Arg2);

public:
    pointer_to_binary_function()
        :ptr(nullptr)
    {}

    explicit pointer_to_binary_function(decltype(ptr) _ptr)
        :ptr(_ptr)
    {}

    Result operator()(Arg1 lhs, Arg2 rhs) const
    {
        return ptr(lhs, rhs);
    }
};


template <class Arg1, class Arg2, class Result>
inline auto
ptr_fun(Result(*ptr)(Arg1, Arg2))
{
    return pointer_to_binary_function<Arg1, Arg2, Result>(ptr);
}

template <class Result, class T>
class mem_fun_t: public unary_function<T*, Result>
{
public:
    explicit mem_fun_t(Result (T::*pf)())
        :f(pf)
    {}

    Result operator()(T *p) const
    {
        return (p->*f)();
    }

private:
    Result (T::*f)();
};


template <class Result, class T>
class const_mem_fun_t: public unary_function<const T*, Result>
{
public:
    explicit const_mem_fun_t(Result (T::*pf)() const)
        :f(pf)
    {}

    Result operator()(const T *p)const
    {
        return (p->*f)();
    }

private:
    Result (T::*f)()const;
};

template <class Result, class T>
class mem_fun_ref_t: public unary_function<T, Result>
{
public:
    explicit mem_fun_ref_t(Result (T::*pf)())
        :f(pf)
    {}

    Result operator()(T &ref)const
    {
        return (ref.*f)();
    }

private:
    Result (T::*f)();
};

template <class Result, class T>
class const_mem_fun_ref_t: public unary_function<T, Result>
{
public:
    explicit const_mem_fun_ref_t(Result (T::*pf)() const)
        :f(pf)
    {}

    Result operator()(const T &ref) const
    {
        return (ref.*f)();
    }
private:
    Result (T::*f)()const;
};


template <class Result, class T, class Arg>
class mem_fun1_t: public binary_function<T*, Arg, Result>
{
public:
    explicit mem_fun1_t(Result (T::*pf)(Arg))
        :f(pf)
    {}

    Result operator()(T *p, Arg x) const
    {
        return (p->*f)(x);
    }
private:
    Result (T::*f)(Arg);
};


template <class Result, class T, class Arg>
class const_mem_fun1_t: public binary_function<const T*, Arg, Result>
{
public:
    explicit const_mem_fun1_t(Result (T::*pf)(Arg) const)
        :f(pf)
    {}

    Result operator()(const T *p, Arg x) const
    {
        return (p->*f)(x);
    }
private:
    Result (T::*f)(Arg) const;
};

template <class Result, class T, class Arg>
class mem_fun1_ref_t: public binary_function<T, Arg, Result>
{
public:
    explicit mem_fun1_ref_t(Result (T::*pf)(Arg))
        :f(pf)
    {}

    Result operator()(T &ref, Arg x) const
    {
        return (ref.*f)(x);
    }

private:
    Result (T::*f)(Arg);
};


template <class Result, class T, class Arg>
class const_mem_fun1_ref_t: public binary_function<T, Arg, Result>
{
public:
    explicit const_mem_fun1_ref_t(Result (T::*pf)(Arg) const)
        :f(pf)
    {}

    Result operator()(const T &r, Arg x) const
    {
        return (r.*f)(x);
    }

private:
    Result (T::*f)(Arg) const;
};


template <class Result, class T>
inline auto
mem_fun(Result (T::*f)())
{
    return mem_fun_t<Result, T>(f);
}

template <class Result, class T>
inline auto
mem_fun(Result (T::*f)() const)
{
    return const_mem_fun_t<Result, T>(f);
};

template <class Result, class T>
inline auto
mem_fun_ref(Result (T::*f)())
{
    return mem_fun_ref_t<Result, T>(f);
};


template <class Result, class T>
inline auto
mem_fun_ref(Result (T::*f)()const)
{
    return const_mem_fun_ref_t<Result, T>(f);
};

template <class Result, class T, class Arg>
inline auto
mem_fun1(Result (T::*f)(Arg))
{
    return mem_fun1_t<Result, T, Arg>(f);
};

template <class Result, class T, class Arg>
inline auto
mem_fun1(Result (T::*f)(Arg) const)
{
    return const_mem_fun1_t<Result, T, Arg>(f);
};

template <class Result, class T, class Arg>
inline auto
mem_fun1_ref(Result (T::*f)(Arg))
{
    return mem_fun1_ref_t<Result, T, Arg>(f);
};

template <class Result, class T, class Arg>
inline auto
mem_fun1_ref(Result (T::*f)(Arg)const)
{
    return const_mem_fun1_ref_t<Result, T, Arg>(f);
};
}// namespace ty

#endif // !_TY_ITERATOR_H