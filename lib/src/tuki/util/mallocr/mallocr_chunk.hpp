#pragma once

/*
	This is a simple generic allocator
	Allocates memory in chunks for fixed size object
	reducing memory fragmentation
	and potentially increasing chache locality
 */

#include <tuki/util/handle.hpp>
#include <utility>
#include <array>
#include <vector>
#include <set>

template <typename T, unsigned CHUNK_SIZE = 32>
class Mallocr_Chunk
{
	friend class iterator;
protected:
	std::vector<std::array<T, CHUNK_SIZE>*> chunks;
	std::set<int> freeSlots;
	std::set<int> occupiedSlots;

public:

	// ITERATOR
	class iterator {
		Mallocr_Chunk* mallocr;
		std::set<int>::iterator it;
	public:
		iterator(Mallocr_Chunk* mallocr, std::set<int>::iterator it)
			: mallocr(mallocr), it(it) {}
		iterator(const iterator& other)
			: mallocr(other.mallocr), it(other.it) {}

		const iterator& operator++() {
			it++;
			return *this;
		}
		bool operator!=(const iterator& other)const {
			return it != other.it;
		}
		T& operator*()const {
			return *mallocr->accessSlot(*it);
		}
		T* operator->()const {
			return mallocr->accessSlot(*it);
		}
	};
	// CONST_ITERATOR
	class const_iterator {
		const Mallocr_Chunk* mallocr;
		std::set<int>::const_iterator it;
	public:
		const_iterator(const Mallocr_Chunk* mallocr, std::set<int>::const_iterator it)
			: mallocr(mallocr), it(it) {}
		const_iterator(const const_iterator& other)
			: mallocr(other.mallocr), it(other.it) {}
		const_iterator(const iterator& other)
			: mallocr(other.mallocr), it(other.it) {}

		const_iterator& operator++() {
			it++;
			return *this;
		}
		bool operator!=(const_iterator& other)const {
			return it != other.it;
		}
		const T& operator*()const {
			return *mallocr->accessSlot(*it);
		}
		const T* operator->()const {
			return mallocr->accessSlot(*it);
		}
	};

	class Handle : public ::handle<T>
	{
		friend class Mallocr_Chunk<T>;
		int index;
		Mallocr_Chunk<T>* mallocr;
	public:
		T & operator*() { return *mallocr->accesSlot(index); }
		const T& operator*()const { return *mallocr->accessSlot(index); }
		T* operator->() { return mallocr->accessSlot(index); }
		const T* operator->()const { return mallocr->accessSlot(index); }

		void destroy() { mallocr->destroy(*this); }
	};

	Mallocr_Chunk() {}

	iterator begin() { return occupiedSlots.begin(); }
	const_iterator begin()const { return occupiedSlots.begin(); }

	iterator end() { return occupiedSlots.end(); }
	const_iterator end()const { return occupiedSlots.end(); }

	Handle create();
	void destroy(Handle& handle);

protected:
	void allocateChunk();
	static std::pair<int, int> getSlotIndex(int index);
	T* accessSlot(int index);
	const T* accessSlot(int index)const;
};

template <typename T, unsigned CHUNK_SIZE>
typename Mallocr_Chunk<T, CHUNK_SIZE>::Handle
Mallocr_Chunk<T, CHUNK_SIZE>::
create()
{
	if (freeSlots.empty()) allocateChunk();
	auto it = freeSlots.begin();
	T* slot = accessSlot(*it);
	freeSlots.erase(it);
	return slot;
}

template <typename T, unsigned CHUNK_SIZE>
void
Mallocr_Chunk<T, CHUNK_SIZE>::
destroy(Mallocr_Chunk<T, CHUNK_SIZE>::Handle& handle)
{
	const int index = (*handle).index;
	bool inOcupied = occupiedSlots.find(index) != occupiedSlots.end();
	assert(occupiedSlots.find(index) != occupiedSlots.end());
	assert(freeSlots.find(index) == freeSlots.end());
	occupiedSlots.erase(index);
	freeSlots.insert(index);
}

template <typename T, unsigned CHUNK_SIZE>
void Mallocr_Chunk<T, CHUNK_SIZE>::allocateChunk()
{
	assert(freeSlots.empty());
	std::array<T, CHUNK_SIZE>* a = new std::array<T, CHUNK_SIZE>;
	const int firstIndex = CHUNK_SIZE * chunks.size();
	const int endIndex = CHUNK_SIZE * chunks.size();
	for (int index = firstIndex; index < endIndex; index++)
	{
		freeSlots.insert(index);
	}
}

template <typename T, unsigned CHUNK_SIZE>
std::pair<int, int>
Mallocr_Chunk<T, CHUNK_SIZE>::
getSlotIndex(int index)
{
	return std::make_pair(index / CHUNK_SIZE, index % CHUNK_SIZE);
}

template <typename T, unsigned CHUNK_SIZE>
T* Mallocr_Chunk<T, CHUNK_SIZE>::accessSlot(int index)
{
	auto pi = getSlotIndex(index);
	return &chunks[pi.first][pi.second];
}

template <typename T, unsigned CHUNK_SIZE>
const T* Mallocr_Chunk<T, CHUNK_SIZE>::accessSlot(int index)const
{
	auto pi = getSlotIndex(index);
	return &chunks[pi.first][pi.second];
}