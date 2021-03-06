﻿#include <array>
#include <cstring>
#include "stdafx.h"
#include "ty_algo.h"
#include "ty_set.h"
#include <algorithm>
#include <iterator>
#include <vector>

using namespace std;


int main()
{

    vector<int> vec;
    auto iter = vec.rbegin();

    iter[3];

    int a[3] = {1, 2, 3};

    copy(begin(a), end(a), ostream_iterator<int>(cout, " ")), cout << endl;
    while (ty::next_permutation(begin(a), end(a)))
        copy(begin(a), end(a), ostream_iterator<int>(cout, " ")), cout << endl;
    copy(begin(a), end(a), ostream_iterator<int>(cout, " "));
    getchar();
    return 0;
}
