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
#include <Windows.h>

#pragma warning( pop )
#endif

template<class Ty, class Enable = void>
struct Allocator;

template<class Ty >
struct Allocator<Ty, typename ::std::enable_if<::std::is_pod<Ty>::value>::type > {
	static Ty* Allocate(::std::size_t size) {
    auto ptr = reinterpret_cast<Ty*>(
#if (defined (WINAPI_ALLOCATOR) && WINAPI_ALLOCATOR)
      ::HeapAlloc( ::GetProcessHeap(), ( _debug ? HEAP_ZERO_MEMORY : 0ul ), ( size * sizeof( Ty ) ) )
#else
      ::malloc( size * sizeof( Ty ) )
#endif
    );

		if (ptr == nullptr)
			throw ::std::bad_alloc{};
		return ptr;
	}

	static void DeAllocate(Ty* const ptr) noexcept {
#if (defined (WINAPI_ALLOCATOR) && WINAPI_ALLOCATOR)
		auto result = ::HeapFree(::GetProcessHeap(), 0ul, reinterpret_cast<void*>(ptr)); assert(result); (void)result;
#else
		::free(reinterpret_cast<void*>(ptr));
#endif
	}
};

template<class Ty >
struct Allocator<Ty, typename ::std::enable_if<!::std::is_pod<Ty>::value>::type > {
	static Ty* Allocate(::std::size_t size) {
		return new Ty[size];
	}

	static void DeAllocate(Ty* const ptr) noexcept {
		delete[] ptr;
	}
};