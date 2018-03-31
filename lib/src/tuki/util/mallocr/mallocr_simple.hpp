#pragma once

#include <set>

template <typename T>
class Mallocr_Simple
{
    std::set<T*> ptrs;
public:
    T* create() {
        T* t = new T;
        ptrs.insert(t);
    }

    void destroy(T* t) {
        assert(ptrs.count() == 1);
        delete t;
        ptrs.erase(t);
    }

    std::set<T>::iterator begin() {
        return ptrs.begin();
    }
    std::set<T>::const_iterator begin()const {
        return ptrs.begin();
    }

    std::set<T>::iterator end() {
        return ptrs.end();
    }
    std::set<T>::const_iterator end()const {
        return ptrs.end();
    }
};