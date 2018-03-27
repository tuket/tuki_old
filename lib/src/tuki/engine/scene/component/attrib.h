#pragma once

#include <type_traits>

template <typename T>
class Attrib
{
	T* t;
	const T& get()const { return t; }
	void set(const T& t) { *(this->t) = t; }
	void operator=(const T& t) { set(t); }
};

template <typename T>
class Attrib<typename T>
