//
// Created by yirany on 2018/4/5.
//

#ifndef THE_ANNOTATED_STL_SOURCES_TY_FUNCTION_H
#define THE_ANNOTATED_STL_SOURCES_TY_FUNCTION_H

namespace ty
{
template <class Arg, class Result>
struct unary_function
{
    using argument_type = Arg;
    using result_type = Result;
};

template <class Arg1, class Arg2, class Result>
struct binary_function
{
    using first_argument_type = Arg1;
    using second_argument_type = Arg2;
    using result_type = Result;
};

template <class T>
struct plus : public binary_function<T, T, T>
{
    T operator()(const T &x, const T &y) const
    {
        return x + y;
    }
};

template <class T>
struct minus : public binary_function<T, T, T>
{
    T operator()(const T &x, const T &y) const
    {
        return x - y;
    }
};

template <class T>
struct multiplies : public binary_function<T, T, T>
{
    T operator()(const T &x, const T &y) const
    {
        return x * y;
    }
};

template <class T>
struct divides : public binary_function<T, T, T>
{
    T operator()(const T &x, const T &y) const
    {
        return x / y;
    }
};

template <class T>
struct modulus : public binary_function<T, T, T>
{
    T operator()(const T &x, const T &y) const
    {
        return x % y;
    }
};

template <class T>
struct negate : public unary_function<T, T>
{
    T operator()(const T &x) const
    {
        return -x;
    }
};

template <class T>
inline
T identity_element(plus<T>)
{
    return T(0);
}

template <class T>
inline
T identity_element(multiplies<T>)
{
    return T(1);
}

template <class T>
struct equal_to : public binary_function<T, T, bool>
{
    bool operator()(const T &x, const T &y) const
    {
        return x == y;
    }
};

template <class T>
struct not_equal_to : public binary_function<T, T, bool>
{
    bool operator()(const T &x, const T&y) const
    {
        return x != y;
    }
};

template <class T>
struct greater : public binary_function<T, T, bool>
{
    bool operator()(const T &x, const T &y) const
    {
        return x > y;
    }
};

template <class T>
struct less : public binary_function<T, T, bool>
{
    bool operator()(const T &x, const T &y) const
    {
        return x < y;
    }
};

template <class T>
struct greater_equal : public binary_function<T, T, bool>
{
    bool operator()(const T &x, const T &y) const
    {
        return x >= y;
    }
};

template <class T>
struct less_equal : public binary_function<T, T, bool>
{
    bool operator() (const T &x, const T &y) const
    {
        return x <= y;
    }
};

template <class T>
struct logical_and : public binary_function<T, T, bool>
{
    bool operator()(const T &x, const T &y) const
    {
        return x && y;
    }
};

template <class T>
struct logical_or : public binary_function<T, T, bool>
{
    bool operator()(const T &x, const T &y) const
    {
        return x || y;
    }
};


template <class T>
struct logical_not : public unary_function<T, bool>
{
    bool operator()(const T &x) const
    {
        return !x;
    }
};

template<typename T>
struct identity : public unary_function<T, T>
{
    const T& operator()(const T &x) const noexcept
    {
        return x;
    }
};

template <class Pair>
struct select1st : public unary_function<Pair, typename Pair::first_type>
{
    const typename Pair::first_type& operator()(const Pair &x) const
    {
        return x.first;
    }
};

template <class Pair>
struct select2nd : public unary_function<Pair, typename Pair::second_type>
{
    const typename Pair::second_type& operator()(const Pair &x) const
    {
        return x.second;
    }
};

template <class Arg1, class Arg2>
struct project1st : public binary_function<Arg1, Arg2, Arg1>
{
    Arg1 operator()(const Arg1 &x, const Arg2 &y) const
    {
        return x;
    }
};

template <class Arg1, class Arg2>
struct project2nd : public binary_function<Arg1, Arg2, Arg2>
{
    Arg2 operator()(const Arg1 &x, const Arg2 &y) const
    {
        return y;
    }
};
}




#endif //THE_ANNOTATED_STL_SOURCES_TY_FUNCTION_H
