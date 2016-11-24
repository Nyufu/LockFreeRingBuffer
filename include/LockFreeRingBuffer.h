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

#include "Allocator.h"
#include <atomic>
#include <cstddef>
#ifdef _MSC_VER
#include <intrin.h>
#endif
#include <utility>

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4324)
// Warning C4324 'LockFreeRingBufferTrivialMovable<Ty,Alloc>': structure was padded due to alignment specifier https://docs.microsoft.com/en-us/cpp/error-messages/compiler-warnings/compiler-warning-level-4-c4324
#endif

static constexpr ::std::size_t cacheline_size = 64;

template<class Ty, class Alloc = Allocator<Ty>>
class LockFreeRingBufferTrivialMovable {
private:
	LockFreeRingBufferTrivialMovable() = delete;

	LockFreeRingBufferTrivialMovable(const LockFreeRingBufferTrivialMovable&) = delete;
	LockFreeRingBufferTrivialMovable(LockFreeRingBufferTrivialMovable&&) = delete;

	LockFreeRingBufferTrivialMovable& operator=(const LockFreeRingBufferTrivialMovable&) = delete;
	LockFreeRingBufferTrivialMovable& operator=(LockFreeRingBufferTrivialMovable&&) = delete;

public:
	LockFreeRingBufferTrivialMovable(::std::size_t size);
	~LockFreeRingBufferTrivialMovable() noexcept;

	bool enqueue(const Ty& value) noexcept;
	bool enqueue(Ty&& value) noexcept(::std::is_nothrow_move_assignable<Ty>::value);

	bool dequeue(Ty& value) noexcept;

	::std::size_t capacity() const noexcept;
	::std::size_t size_approx() const noexcept;

protected:
	struct Pack {
		const ::std::size_t capacity;
		Ty* const			data;
	};

	static_assert(sizeof(Pack) < cacheline_size, "The capacity and the data pointer don't fit into a cache line!");

protected:
	alignas(cacheline_size)Pack					pack;

	alignas(cacheline_size)::std::atomic_size_t reserver;
	alignas(cacheline_size)::std::atomic_size_t last;
	alignas(cacheline_size)::std::atomic_size_t first;
};

template<class Ty, class Alloc = Allocator<Ty>>
class LockFreeRingBufferNonTrivialMovable : public LockFreeRingBufferTrivialMovable<Ty, Alloc> {
private:
	LockFreeRingBufferNonTrivialMovable() = delete;

	LockFreeRingBufferNonTrivialMovable(const LockFreeRingBufferNonTrivialMovable&) = delete;
	LockFreeRingBufferNonTrivialMovable(LockFreeRingBufferNonTrivialMovable&&) = delete;

	LockFreeRingBufferNonTrivialMovable& operator=(const LockFreeRingBufferNonTrivialMovable&) = delete;
	LockFreeRingBufferNonTrivialMovable& operator=(LockFreeRingBufferNonTrivialMovable&&) = delete;

protected:
	using MyBase = LockFreeRingBufferTrivialMovable<Ty, Alloc>;

public:
	LockFreeRingBufferNonTrivialMovable(::std::size_t size);

	bool dequeue(Ty& value) noexcept(::std::is_nothrow_move_assignable<Ty>::value);

protected:
	alignas(cacheline_size)::std::atomic_size_t lastReserver;
};

#if defined(__clang__) && __clang__
#define lzcnt64(x) __builtin_clzll(x)
#else
#define lzcnt64(x) __lzcnt64(x)
#endif

template<class Ty, class Alloc>
LockFreeRingBufferTrivialMovable<Ty, Alloc>::LockFreeRingBufferTrivialMovable(::std::size_t size)
	: pack{ { size ? (2ul << lzcnt64(size)) - 1 : 0ul },
			{ size ? Alloc::Allocate(2ull << lzcnt64(size)) : nullptr } }
	, reserver{ 0 }
	, last{ 0 }
	, first{ 0 } {
	assert((size != 0 && pack.data != nullptr) || (size == 0 && pack.data == nullptr));
}

template<class Ty, class Alloc>
LockFreeRingBufferTrivialMovable<Ty, Alloc>::~LockFreeRingBufferTrivialMovable() noexcept {
	Alloc::DeAllocate(pack.data);
}

template<class Ty, class Alloc>
bool LockFreeRingBufferTrivialMovable<Ty, Alloc>::enqueue(const Ty& value) noexcept {
	const auto mask = pack.capacity;

	auto candidate = reserver.load();
	decltype(candidate) incremented = 0;

	do {
		incremented = (candidate + 1) & mask;

		if (last.load() == incremented)
			return false;

	} while (!reserver.compare_exchange_weak(candidate, incremented));

	auto reserved = candidate;

	pack.data[reserved] = value;

	for (const auto savedReserver = reserved; !first.compare_exchange_weak(reserved, incremented); reserved = savedReserver)
		;

	return true;
}

template<class Ty, class Alloc>
bool LockFreeRingBufferTrivialMovable<Ty, Alloc>::enqueue(Ty&& value) noexcept(::std::is_nothrow_move_assignable<Ty>::value) {
	const auto mask = pack.capacity;

	auto candidate = reserver.load();
	decltype(candidate) incremented = 0;

	do {
		incremented = (candidate + 1) & mask;

		if (last.load() == incremented)
			return false;

	} while (!reserver.compare_exchange_weak(candidate, incremented));

	auto reserved = candidate;

	pack.data[reserved] = ::std::forward<Ty>(value);

	for (const auto savedReserver = reserved; !first.compare_exchange_weak(reserved, incremented); reserved = savedReserver)
		;

	return true;
}

template<class Ty, class Alloc>
bool LockFreeRingBufferTrivialMovable<Ty, Alloc>::dequeue(Ty& value) noexcept {
	const auto mask = pack.capacity;
	const auto ptr = pack.data;

	auto currentLast = last.load();

	do {
		if (first.load() == currentLast)
			return false;

		value = ptr[currentLast];

	} while (!last.compare_exchange_weak(currentLast, (currentLast + 1) & mask));

	return true;
}

template<class Ty, class Alloc>
::std::size_t LockFreeRingBufferTrivialMovable<Ty, Alloc>::capacity() const noexcept {
	return pack.capacity;
}

template<class Ty, class Alloc>
::std::size_t LockFreeRingBufferTrivialMovable<Ty, Alloc>::size_approx() const noexcept {
	return first.load(::std::memory_order_relaxed) - last.load(::std::memory_order_relaxed);
}

template<class Ty, class Alloc>
LockFreeRingBufferNonTrivialMovable<Ty, Alloc>::LockFreeRingBufferNonTrivialMovable(::std::size_t size)
	: MyBase(size),
	lastReserver{ 0 } {
}

template<class Ty, class Alloc>
bool LockFreeRingBufferNonTrivialMovable<Ty, Alloc>::dequeue(Ty& value) noexcept(::std::is_nothrow_move_assignable<Ty>::value) {
	const auto mask = MyBase::pack.capacity;

	auto candidate = lastReserver.load();
	decltype(candidate) incremented = 0;

	do {
		if (MyBase::first.load() == candidate)
			return false;

		incremented = (candidate + 1) & mask;

	} while (!lastReserver.compare_exchange_weak(candidate, incremented));

	auto reserved = candidate;

	value = ::std::move(MyBase::pack.data[reserved]);

	for (const auto savedReserved = reserved; !MyBase::last.compare_exchange_weak(reserved, incremented); reserved = savedReserved)
		;

	return true;
}

template<class Ty, class Alloc = Allocator<Ty>>
using LockFreeRingBuffer =
typename ::std::conditional <
	::std::is_trivially_move_assignable<Ty>::value,
	LockFreeRingBufferTrivialMovable<Ty, Alloc>,
	LockFreeRingBufferNonTrivialMovable<Ty, Alloc>
>::type;

static_assert((sizeof(LockFreeRingBuffer<void*>) & (cacheline_size - 1)) == 0, "The LockFreeRingBuffer has unexpected size!");

#ifdef _MSC_VER
#pragma warning(pop)
#endif