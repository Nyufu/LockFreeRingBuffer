// MIT License
//
// Copyright (c) 2016 Andris Nyiscsák
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once

#include "Config.h"
#include <intrin.h>
#include <atomic>
#include "Allocator.h"

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4711)
#endif

template<class _Ty, class _Alloc = Allocator<_Ty>>
class LockFreeRingBufferTrivialMovable {
private:
	LockFreeRingBufferTrivialMovable() = delete;

	LockFreeRingBufferTrivialMovable(const LockFreeRingBufferTrivialMovable&) = delete;
	LockFreeRingBufferTrivialMovable(LockFreeRingBufferTrivialMovable&&) = delete;

	LockFreeRingBufferTrivialMovable& operator=(const LockFreeRingBufferTrivialMovable&) = delete;
	LockFreeRingBufferTrivialMovable& operator=(LockFreeRingBufferTrivialMovable&&) = delete;

public:
	LockFreeRingBufferTrivialMovable(uint32_t size) noexcept;
	~LockFreeRingBufferTrivialMovable() noexcept;

	bool enqueue(const _Ty& value) noexcept;
	bool enqueue(_Ty&& value) noexcept;

	bool try_dequeue(_Ty& value) noexcept;

	size_t capacity() const noexcept;
	size_t size_approx() const noexcept;

protected:
	const size_t capacity_;

	_Ty* const data;

	_STD atomic_uint64_t reserver;
	_STD atomic_uint64_t last;
	_STD atomic_uint64_t first;
};

template<class _Ty, class _Alloc = Allocator<_Ty>>
class LockFreeRingBufferNonTrivialMovable : public LockFreeRingBufferTrivialMovable<_Ty, _Alloc> {
private:
	LockFreeRingBufferNonTrivialMovable() = delete;

	LockFreeRingBufferNonTrivialMovable(const LockFreeRingBufferNonTrivialMovable&) = delete;
	LockFreeRingBufferNonTrivialMovable(LockFreeRingBufferNonTrivialMovable&&) = delete;

	LockFreeRingBufferNonTrivialMovable& operator=(const LockFreeRingBufferNonTrivialMovable&) = delete;
	LockFreeRingBufferNonTrivialMovable& operator=(LockFreeRingBufferNonTrivialMovable&&) = delete;

protected:
	using MyBase = LockFreeRingBufferTrivialMovable<_Ty, _Alloc>;

public:
	LockFreeRingBufferNonTrivialMovable(uint32_t size) noexcept;

	bool try_dequeue(_Ty& value) noexcept;

protected:
	_STD atomic_uint64_t lastReserver;
};

#if defined(__clang__) && __clang__
#define lzcnt64(x) __builtin_clzll(x)
#else
#define lzcnt64(x) __lzcnt64(x)
#endif

template<class _Ty, class _Alloc>
LockFreeRingBufferTrivialMovable<_Ty, _Alloc>::LockFreeRingBufferTrivialMovable(uint32_t size) noexcept
	: capacity_{ (2ull << lzcnt64(size)) - 1 }
	, data{ _Alloc::Allocate(2ull << lzcnt64(size)) }
	, reserver{ 0 }
	, last{ 0 }
	, first{ 0 } {
	assert(data != nullptr);
}

template<class _Ty, class _Alloc>
LockFreeRingBufferTrivialMovable<_Ty, _Alloc>::~LockFreeRingBufferTrivialMovable() noexcept {
	_Alloc::DeAllocate(data);
}

template<class _Ty, class _Alloc>
bool LockFreeRingBufferTrivialMovable<_Ty, _Alloc>::enqueue(const _Ty& value) noexcept {
	const auto mask = capacity_;

	auto candidate = reserver.load();
	decltype(candidate) incremented = 0;

	do {
		incremented = (candidate + 1) & mask;

		if (last.load() == incremented)
			return false;

	} while (!reserver.compare_exchange_weak(candidate, incremented));

	auto reserved = candidate;

	data[reserved] = value;

	for (auto savedReserver = reserved; !first.compare_exchange_weak(reserved, incremented); reserved = savedReserver)
		;

	return true;
}

template<class _Ty, class _Alloc>
bool LockFreeRingBufferTrivialMovable<_Ty, _Alloc>::enqueue(_Ty&& value) noexcept {
	const auto mask = capacity_;

	auto candidate = reserver.load();
	decltype(candidate) incremented = 0;

	do {
		incremented = (candidate + 1) & mask;

		if (last.load() == incremented)
			return false;

	} while (!reserver.compare_exchange_weak(candidate, incremented));

	auto reserved = candidate;

	data[reserved] = _STD forward<_Ty>(value);

	for (auto savedReserver = reserved; !first.compare_exchange_weak(reserved, incremented); reserved = savedReserver)
		;

	return true;
}

template<class _Ty, class _Alloc>
bool LockFreeRingBufferTrivialMovable<_Ty, _Alloc>::try_dequeue(_Ty& value) noexcept {
	const auto mask = capacity_;
	const auto ptr = data;

	auto currentLast = last.load();

	do {
		if (first.load() == currentLast)
			return false;

		value = ptr[currentLast];

	} while (!last.compare_exchange_weak(currentLast, (currentLast + 1) & mask));

	return true;
}

template<class _Ty, class _Alloc>
size_t LockFreeRingBufferTrivialMovable<_Ty, _Alloc>::capacity() const noexcept {
	return capacity_;
}

template<class _Ty, class _Alloc>
size_t LockFreeRingBufferTrivialMovable<_Ty, _Alloc>::size_approx() const noexcept {
	return first.load(_STD memory_order_relaxed) - last.load(_STD memory_order_relaxed);
}

template<class _Ty, class _Alloc>
LockFreeRingBufferNonTrivialMovable<_Ty, _Alloc>::LockFreeRingBufferNonTrivialMovable(uint32_t size) noexcept
	: MyBase(size),
	lastReserver{ 0 } {
}

template<class _Ty, class _Alloc>
bool LockFreeRingBufferNonTrivialMovable<_Ty, _Alloc>::try_dequeue(_Ty& value) noexcept {
	const auto mask = MyBase::capacity_;

	auto candidate = lastReserver.load();
	decltype(candidate) incremented = 0;

	do {
		incremented = (candidate + 1) & mask;

		if (MyBase::first.load() == incremented)
			return false;

	} while (!lastReserver.compare_exchange_weak(candidate, incremented));

	auto reserved = candidate;

	value = _STD move(MyBase::data[reserved]);

	for (auto savedReserved = reserved; !MyBase::last.compare_exchange_weak(reserved, incremented); reserved = savedReserved)
		;

	return true;
}

template<class _Ty, class _Alloc = Allocator<_Ty>>
using LockFreeRingBuffer =
_STD conditional_t <
	_STD is_trivially_move_assignable<_Ty>::value,
	LockFreeRingBufferTrivialMovable<_Ty, _Alloc>,
	LockFreeRingBufferNonTrivialMovable<_Ty, _Alloc>
>;

#ifdef _MSC_VER
#pragma warning(pop)
#endif