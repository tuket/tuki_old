#pragma once

#include <vector>
#include <algorithm>
#include <functional>
#include <cassert>
#include <numeric>

template <typename T, typename Compare>
void getSortPermutation(
    std::vector<unsigned>& out,
    const std::vector<T>& v,
    Compare compare = std::less<T>())
{
    out.resize(v.size());
    std::iota(out.begin(), out.end(), 0);
    
    std::sort(out.begin(), out.end(),
        [&](unsigned i, unsigned j){ return compare(v[i], v[j]); });
}

template <typename T>
void applyPermutation(
    const std::vector<unsigned>& order,
    std::vector<T>& t)
{
    assert(order.size() == t.size());
    std::vector<T> st(t.size());
    for(unsigned i=0; i<t.size(); i++)
    {
        st[i] = t[order[i]];
    }
    t = st;
}

template <typename T, typename... S>
void applyPermutation(
    const std::vector<unsigned>& order,
    std::vector<T>& t,
    std::vector<S>&... s)
{
    applyPermutation(order, t);
    applyPermutation(order, s...);
}

// sort multiple vectors using the criteria of the first one
template<typename T, typename Compare, typename... SS>
void sortVectors(
    const std::vector<T>& t,
    Compare comp,
    std::vector<SS>&... ss)
{
    std::vector<unsigned> order;
    getSortPermutation(order, t, comp);
    applyPermutation(order, ss...);
}

// make less verbose for the usual ascending order
template<typename T, typename... SS>
void sortVectorsAscending(
    const std::vector<T>& t,
    std::vector<SS>&... ss)
{
    sortVectors(t, std::less<T>(), ss...);
}