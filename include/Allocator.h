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
#include <cassert>
#include <type_traits>

#if (defined (WINAPI_ALLOCATOR) && WINAPI_ALLOCATOR)
#pragma warning( push, 0 )

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>

#pragma warning( pop )
#endif

template<class _Ty>
struct Allocator {
	_CONSTEXPR14 static _Ty* Allocate(size_t size) noexcept {
#if (defined (WINAPI_ALLOCATOR) && WINAPI_ALLOCATOR)
		return reinterpret_cast<_Ty*>(::HeapAlloc(::GetProcessHeap(), (_debug ? HEAP_ZERO_MEMORY : 0ul), (size * sizeof(_Ty))));
#else
		return ::new (_STD nothrow) _Ty[size];
#endif
	}

	template<class _Rty>
	_CONSTEXPR14 static _Rty Allocate(size_t size) noexcept {
#if (defined (WINAPI_ALLOCATOR) && WINAPI_ALLOCATOR)
		return reinterpret_cast<_Rty>(::HeapAlloc(::GetProcessHeap(), (_debug ? HEAP_ZERO_MEMORY : 0ul), (size * sizeof(_Ty))));
#else
		return reinterpret_cast<_Rty>(::new (_STD nothrow) _Ty[size]);
#endif
	}

	_CONSTEXPR14 static void DeAllocate(_Ty* const ptr) noexcept {
#if (defined (WINAPI_ALLOCATOR) && WINAPI_ALLOCATOR)
		auto result = ::HeapFree(::GetProcessHeap(), 0ul, reinterpret_cast<void*>(ptr)); assert(result); (void)result;
#else
		::delete[] ptr;
#endif
	}

	template<class _Rty>
	_CONSTEXPR14 static void DeAllocate(_Rty const ptr) noexcept {
#if (defined (WINAPI_ALLOCATOR) && WINAPI_ALLOCATOR)
		auto result = ::HeapFree(::GetProcessHeap(), 0ul, reinterpret_cast<void*>(ptr)); assert(result); (void)result;
#else
		::delete[] reinterpret_cast<_Ty*>(ptr);
#endif
	}
};