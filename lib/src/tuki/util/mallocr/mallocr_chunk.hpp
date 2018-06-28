#pragma once

/*
	This is a simple generic allocator
	Allocates memory in chunks for fixed size objects
	reducing memory fragmentation
	and potentially increasing chache locality
 */

#include <utility>
#include <array>
#include <vector>
#include <set>
#include <stdexcpt.h>
#include <memory>

template <typename T, size_t CHUNK_SIZE = 32>
class Mallocr_Chunk
{
	friend class iterator;
	friend class Deleter;
public:

	// ITERATOR
	class iterator {
		Mallocr_Chunk* mallocr;
		std::set<size_t>::iterator it;
	public:
		iterator(Mallocr_Chunk* mallocr, std::set<size_t>::iterator it)
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
		std::set<size_t>::const_iterator it;
	public:
		const_iterator(const Mallocr_Chunk* mallocr, std::set<size_t>::const_iterator it)
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

	class Deleter {
		size_t index;
		Mallocr_Chunk* mallocr;
	public:
		void operator()(T* t) { mallocr->destroy(index); }
	};

	Mallocr_Chunk() {}

	iterator begin() { return occupiedSlots.begin(); }
	const_iterator begin()const { return occupiedSlots.begin(); }

	iterator end() { return occupiedSlots.end(); }
	const_iterator end()const { return occupiedSlots.end(); }

	std::unique_ptr<T> create() {
		if (freeSlots.empty()) allocateChunk();
		auto it = freeSlots.begin();
		Slot* slot = accessSlot(*it);
		slot->d.index = *it;
		slot->d.mallocr = this;
		freeSlots.erase(it);
		return std::unique_ptr(&slot->t, &slot->d);
	}

protected:
	struct Slot { Deleter d; T t; };
	// DATA
	std::vector<std::array<Slot, CHUNK_SIZE>*> chunks;
	std::set<size_t> freeSlots;
	std::set<size_t> occupiedSlots;

	// FUNCTIONS
	void allocateChunk();
	static std::pair<size_t, size_t> getSlotIndex(size_t index) {
		return std::make_pair(index / CHUNK_SIZE, index % CHUNK_SIZE);
	}
	Slot* accessSlot(size_t index) {
		auto pi = getSlotIndex(index);
		return &chunks[pi.first][pi.second];
	}
	const Slot* accessSlot(size_t index)const {
		auto pi = getSlotIndex(index);
		return &chunks[pi.first][pi.second];
	}
	void destroy(size_t index);
};

template <typename T, size_t CHUNK_SIZE>
void
Mallocr_Chunk<T, CHUNK_SIZE>::
destroy(size_t index)
{
	bool inOcupied = occupiedSlots.find(index) != occupiedSlots.end();
	assert(occupiedSlots.find(index) != occupiedSlots.end());
	assert(freeSlots.find(index) == freeSlots.end());
	occupiedSlots.erase(index);
	freeSlots.insert(index);
}

template <typename T, size_t CHUNK_SIZE>
void Mallocr_Chunk<T, CHUNK_SIZE>::allocateChunk()
{
	assert(freeSlots.empty());
	std::array<Slot, CHUNK_SIZE>* a = new std::array<Slot, CHUNK_SIZE>;
	const size_t firstIndex = CHUNK_SIZE * chunks.size();
	const size_t endIndex = CHUNK_SIZE * chunks.size();
	for (size_t index = firstIndex; index < endIndex; index++) {
		freeSlots.insert(index);
	}
}
