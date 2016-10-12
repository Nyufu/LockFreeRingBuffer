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

#include <atomic>
#include <intrin.h>
#include <cassert>
#include "Config.h"
#include "WinAPI_inline.h"
#include "Utilities.h"

template<class _Ty>
class LockFreeRingBuffer {

	static_assert(std::is_pod_v<_Ty>, "This ringbufffer stil doesn't work with non POD types.");

private:
	LockFreeRingBuffer() = delete;

	LockFreeRingBuffer(const LockFreeRingBuffer&) = delete;
	LockFreeRingBuffer(LockFreeRingBuffer&&) = delete;

	LockFreeRingBuffer& operator=(const LockFreeRingBuffer&) = delete;
	LockFreeRingBuffer& operator=(LockFreeRingBuffer&&) = delete;

public:
	_CONSTEXPR14 LockFreeRingBuffer(size_t size) noexcept;
	~LockFreeRingBuffer() noexcept;

	_CONSTEXPR14 bool enqueue(_Ty value) noexcept;

	_CONSTEXPR14 bool try_dequeue(_Ty& value) noexcept;

	_CONSTEXPR14 size_t size_approx() const noexcept;

#if (defined (_TEST) && _TEST)

  _CONSTEXPR14 auto max() const noexcept
  {
    return last.load( std::memory_order_relaxed );
  }

#endif

protected:
	const size_t capacity;

	_Ty* const data;

	std::atomic_uint64_t reserver;
	std::atomic_uint64_t first;
	std::atomic_uint64_t last;
};

template<class _Ty>
_CONSTEXPR14 LockFreeRingBuffer<_Ty>::LockFreeRingBuffer(size_t size) noexcept
	: capacity{ (2ull << __lzcnt64(size)) - 1 }
	, data{ reinterpret_cast<decltype(data)>(::HeapAlloc(GetProcessHeap_(), (_debug ? HEAP_ZERO_MEMORY : 0), (capacity + 1) * sizeof(_Ty))) }
	, reserver{ 0 }
	, first{ 0 }
	, last{ 0 } {
	assert(size > 0);
	assert(data);
}

template<class _Ty>
LockFreeRingBuffer<_Ty>::~LockFreeRingBuffer() noexcept {
	auto result = ::HeapFree(GetProcessHeap_(), 0, reinterpret_cast<void*>(data)); assert(result); UNUSED(result);
}

template<class _Ty>
_CONSTEXPR14 bool LockFreeRingBuffer<_Ty>::enqueue(_Ty value) noexcept {
	uint64_t currentReserver = 0;
	uint64_t reserverPlusOn = 0;

	do {
		currentReserver = reserver.load(std::memory_order_acquire);
		reserverPlusOn = currentReserver + 1;

		auto currentLast = last.load(std::memory_order_acquire);

		if ((reserverPlusOn & capacity) == (currentLast & capacity))
			return false;

	} while (!reserver.compare_exchange_weak(currentReserver, reserverPlusOn, std::memory_order_release, std::memory_order_relaxed));

	data[currentReserver & capacity] = std::move(value);

	auto expectedFirst = currentReserver;

	while (!first.compare_exchange_weak(expectedFirst, reserverPlusOn, std::memory_order_release, std::memory_order_relaxed))
		expectedFirst = currentReserver;

	return true;
}

template<class _Ty>
_CONSTEXPR14 bool LockFreeRingBuffer<_Ty>::try_dequeue(_Ty& value) noexcept {
	auto currentLast = last.load(std::memory_order_acquire);

	do {
		if (first.load(std::memory_order_acquire) == currentLast)
			return false;

		value = std::move(data[currentLast & capacity]);

	} while (!last.compare_exchange_weak(currentLast, currentLast + 1, std::memory_order_release, std::memory_order_relaxed));

	return true;
}

template<class _Ty>
_CONSTEXPR14 size_t LockFreeRingBuffer<_Ty>::size_approx() const noexcept {
	return first.load(std::memory_order_relaxed) - last.load(std::memory_order_relaxed);
}
