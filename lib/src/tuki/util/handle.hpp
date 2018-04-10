#pragma once

#include <cassert>

template <typename T>
struct handle {
	virtual T& operator*() = 0;
	virtual const T& operator*()const = 0;
	virtual T* operator->() = 0;
	virtual const T* operator->()const = 0;
};

template <typename T>
class ptr : public handle<T>
{
	T* t;
public:
	ptr() : t(nullptr) {}
	ptr(T* t) : t(t) {}
	ptr(ptr<T> other) : t(other.t) {}

	ptr<T> operator=(ptr<T> other) {
		t = other.t;
		return *this;
	}

	bool operator==(ptr<T> other)const { return t == other.t; }

	T& operator*() {
		assert(t);
		return *t;
	}
	const T& operator*()const {
		assert(t);
		return *t;
	}

	T* operator->() {
		assert(t);
		return t;
	}
	const T* operator->()const {
		assert(t);
		return t;
	}

	void free() {
		assert(t);
		delete t;
		t = nullptr;
	}
};