// STL_note.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <iostream>
#include <deque>

typedef void return_type;
typedef void(*function_type)();

// this function return a function pointer like : return_type func_pointer(double);
// require an int and a function_type parameter
return_type(*function_name(int i, function_type func)) (double)
{
    return 0;
}

#include <vector>
#include <typeinfo>
#include <iostream> 
#include <string>
#include <queue>
#include <forward_list>
#include <ctime>
#include <array>
#include <set>
#include <boost\type_index.hpp>
using namespace std;

template <typename Container, typename size_type>
decltype(auto)
getElementIn(Container &c, size_type i)
{
    return c[i];
}

decltype(auto) getVector() { return vector<int>(10); }

int main()
{
    using boost::typeindex::type_id_with_cvr;

    const int i = 2;
    const int &ref = i;
    auto *poi = &i;

    cout << typeid(i).name() << endl;
    cout << typeid(ref).name() << endl;
    cout << typeid(poi).name() << endl;

    cout << type_id_with_cvr<const int &>().pretty_name() << endl;

    system("pause");
    return 0;
}
