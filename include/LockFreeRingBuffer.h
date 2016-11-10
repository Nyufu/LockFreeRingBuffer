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
	_CONSTEXPR14 LockFreeRingBufferTrivialMovable(uint32_t size) noexcept;
	~LockFreeRingBufferTrivialMovable() noexcept;

	_CONSTEXPR14 bool enqueue(_Ty& value) noexcept;
	_CONSTEXPR14 bool enqueue(_Ty&& value) noexcept;

	_CONSTEXPR14 bool try_dequeue(_Ty& value) noexcept;

	_CONSTEXPR14 size_t size_approx() const noexcept;

#if (defined (_TEST) && _TEST)

	_CONSTEXPR14 auto max() const noexcept {
		return last.load(_STD memory_order_relaxed);
	}

#endif

protected:
	const size_t capacity;

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
	_CONSTEXPR14 LockFreeRingBufferNonTrivialMovable(uint32_t size) noexcept;

	_CONSTEXPR14 bool try_dequeue(_Ty& value) noexcept;

protected:
	_STD atomic_uint64_t lastReserver;
};

#if defined(__clang__) && __clang__
#define lzcnt64(x) __builtin_clzll(x)
#else
#define lzcnt64(x) __lzcnt64(x)
#endif

template<class _Ty, class _Alloc>
_CONSTEXPR14 LockFreeRingBufferTrivialMovable<_Ty, _Alloc>::LockFreeRingBufferTrivialMovable(uint32_t size) noexcept
	: capacity{ (2ull << lzcnt64(size)) - 1 }
	, data{ size ? _Alloc::Allocate(2ull << lzcnt64(size)) : nullptr }
	, reserver{ 0 }
	, last{ 0 }
	, first{ 0 } {
	assert((size != 0 && data != nullptr) || (size == 0 && data == nullptr));
}

template<class _Ty, class _Alloc>
LockFreeRingBufferTrivialMovable<_Ty, _Alloc>::~LockFreeRingBufferTrivialMovable() noexcept {
	_Alloc::DeAllocate(data);
}

template<class _Ty, class _Alloc>
_CONSTEXPR14 bool LockFreeRingBufferTrivialMovable<_Ty, _Alloc>::enqueue(_Ty& value) noexcept {
	uint64_t currentReserver = 0;
	uint64_t reserverPlusOne = 0;

	const auto mask = capacity;

	do {
		currentReserver = reserver.load(_STD memory_order_acquire);
		auto currentLast = last.load(_STD memory_order_acquire);

		reserverPlusOne = currentReserver + 1;

		if ((reserverPlusOne & mask) == (currentLast & mask))
			return false;

	} while (!reserver.compare_exchange_weak(currentReserver, reserverPlusOne, _STD memory_order_release, _STD memory_order_relaxed));

	data[currentReserver & mask] = _STD forward<_Ty>(value);

	auto expectedFirst = currentReserver;

	while (!first.compare_exchange_weak(expectedFirst, reserverPlusOne, _STD memory_order_release, _STD memory_order_relaxed))
		expectedFirst = currentReserver;

	return true;
}

template<class _Ty, class _Alloc>
_CONSTEXPR14 bool LockFreeRingBufferTrivialMovable<_Ty, _Alloc>::enqueue(_Ty&& value) noexcept {
	uint64_t currentReserver = 0;
	uint64_t reserverPlusOn = 0;

	const auto mask = capacity;

	do {
		currentReserver = reserver.load(_STD memory_order_acquire);
		auto currentLast = last.load(_STD memory_order_acquire);

		reserverPlusOn = currentReserver + 1;

		if ((reserverPlusOn & mask) == (currentLast & mask))
			return false;

	} while (!reserver.compare_exchange_weak(currentReserver, reserverPlusOn, _STD memory_order_release, _STD memory_order_relaxed));

	data[currentReserver & mask] = _STD forward<_Ty>(value);

	auto expectedFirst = currentReserver;

	while (!first.compare_exchange_weak(expectedFirst, reserverPlusOn, _STD memory_order_release, _STD memory_order_relaxed))
		expectedFirst = currentReserver;

	return true;
}

template<class _Ty, class _Alloc>
_CONSTEXPR14 bool LockFreeRingBufferTrivialMovable<_Ty, _Alloc>::try_dequeue(_Ty& value) noexcept {
	const auto mask = capacity;
	const auto ptr = data;

	auto currentLast = last.load(_STD memory_order_acquire);

	do {
		if (first.load(_STD memory_order_acquire) == currentLast)
			return false;

		value = ptr[currentLast & mask];

	} while (!last.compare_exchange_weak(currentLast, currentLast + 1, _STD memory_order_release, _STD memory_order_relaxed));

	return true;
}

template<class _Ty, class _Alloc>
_CONSTEXPR14 size_t LockFreeRingBufferTrivialMovable<_Ty, _Alloc>::size_approx() const noexcept {
	return first.load(_STD memory_order_relaxed) - last.load(_STD memory_order_relaxed);
}

template<class _Ty, class _Alloc>
_CONSTEXPR14 LockFreeRingBufferNonTrivialMovable<_Ty, _Alloc>::LockFreeRingBufferNonTrivialMovable(uint32_t size) noexcept
	: MyBase(size),
	lastReserver{ 0 } {
}

template<class _Ty, class _Alloc>
_CONSTEXPR14 bool LockFreeRingBufferNonTrivialMovable<_Ty, _Alloc>::try_dequeue(_Ty& value) noexcept {
	uint64_t currentReserver = 0;
	uint64_t reserverPlusOn = 0;

	const auto mask = MyBase::capacity;

	do {
		auto currentFist = MyBase::first.load(_STD memory_order_acquire);
		currentReserver = lastReserver.load(_STD memory_order_acquire);

		if ((currentFist & mask) == (currentReserver & mask))
			return false;

		reserverPlusOn = currentReserver + 1;

	} while (!lastReserver.compare_exchange_weak(currentReserver, reserverPlusOn, _STD memory_order_release, _STD memory_order_relaxed));

	value = _STD move(MyBase::data[currentReserver & mask]);

	auto expectedLast = currentReserver;

	while (!MyBase::last.compare_exchange_weak(expectedLast, reserverPlusOn, _STD memory_order_release, _STD memory_order_relaxed))
		expectedLast = currentReserver;

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