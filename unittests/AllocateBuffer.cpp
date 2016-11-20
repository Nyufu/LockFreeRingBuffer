#pragma warning( push, 0 )

#include "gtest\gtest.h"
#include <memory>

#pragma warning( pop )

#include "LockFreeRingBuffer.h"

namespace Test_AllocateBuffer_Byte_0_size {

int32_t constructorCallCounter = 0;
int32_t destructorCallCounter = 0;

template<class _Ty>
struct MockAllocator {
	static _Ty* Allocate(size_t size) noexcept {
		constructorCallCounter++;
		return (reinterpret_cast<_Ty*>(0xF0));
	}

	static void DeAllocate(_Ty* const ptr) noexcept {
		EXPECT_EQ(ptr, nullptr);
		destructorCallCounter++;
	}
};

TEST(AllocateBuffer, Byte_size_0) {
	{
		LockFreeRingBuffer<uint8_t, MockAllocator<uint8_t>> ringBuffer(0);
		EXPECT_EQ(ringBuffer.capacity(), 0);
	}

	EXPECT_EQ(constructorCallCounter, 0);
	EXPECT_EQ(destructorCallCounter, 1);
}

}

namespace Test_AllocateBuffer_Byte_0_size_ExternalArg {

int32_t constructorCallCounter = 0;
int32_t destructorCallCounter = 0;

template<class _Ty>
struct MockAllocator {
	static _Ty* Allocate(size_t size) noexcept {
		constructorCallCounter++;
		return (reinterpret_cast<_Ty*>(0xF0));
	}

	static void DeAllocate(_Ty* const ptr) noexcept {
		EXPECT_EQ(ptr, nullptr);
		destructorCallCounter++;
	}
};

__declspec(noinline) int Size() noexcept {
	return 0;
}

TEST(AllocateBuffer, Byte_size_0_ExternalArg) {
	{
		LockFreeRingBuffer<uint8_t, MockAllocator<uint8_t>> ringBuffer(Size());
		EXPECT_EQ(ringBuffer.capacity(), 0);
	}

	EXPECT_EQ(constructorCallCounter, 0);
	EXPECT_EQ(destructorCallCounter, 1);
}

}

namespace Test_AllocateBuffer_Byte_size_1 {

int32_t constructorCallCounter = 0;
int32_t destructorCallCounter = 0;

template<class _Ty>
struct MockAllocator {
	static _Ty* Allocate(size_t size) noexcept {
		constructorCallCounter++;
		EXPECT_EQ(size, 2);
		return (reinterpret_cast<_Ty*>(0xF0));
	}

	static void DeAllocate(_Ty* const ptr) noexcept {
		destructorCallCounter++;
		EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr), 0xF0);
	}
};

TEST(AllocateBuffer, Byte_size_1) {
	{
		LockFreeRingBuffer<uint8_t, MockAllocator<uint8_t>> ringBuffer(1);
		EXPECT_EQ(ringBuffer.capacity(), 1);
	}

	EXPECT_EQ(constructorCallCounter, 1);
	EXPECT_EQ(destructorCallCounter, 1);
}

}

namespace Test_AllocateBuffer_Byte_size_1000 {

int32_t constructorCallCounter = 0;
int32_t destructorCallCounter = 0;

template<class _Ty>
struct MockAllocator {
	static _Ty* Allocate(size_t size) noexcept {
		constructorCallCounter++;
		EXPECT_EQ(size, 1024);
		return (reinterpret_cast<_Ty*>(0xF0));
	}

	static void DeAllocate(_Ty* const ptr) noexcept {
		destructorCallCounter++;
		EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr), 0xF0);
	}
};

TEST(AllocateBuffer, Byte_size_1000) {
	{
		LockFreeRingBuffer<uint8_t, MockAllocator<uint8_t>> ringBuffer(1000);
		EXPECT_EQ(ringBuffer.capacity(), 1023);
	}

	EXPECT_EQ(constructorCallCounter, 1);
	EXPECT_EQ(destructorCallCounter, 1);
}

}

namespace Test_AllocateBuffer_Byte_size_1500_ExternalArg {

int32_t constructorCallCounter = 0;
int32_t destructorCallCounter = 0;

template<class _Ty>
struct MockAllocator {
	static _Ty* Allocate(size_t size) noexcept {
		constructorCallCounter++;
		EXPECT_EQ(size, 2048);
		return (reinterpret_cast<_Ty*>(0xF0));
	}

	static void DeAllocate(_Ty* const ptr) noexcept {
		destructorCallCounter++;
		EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr), 0xF0);
	}
};

__declspec(noinline) int Size() noexcept {
	return 1500;
}

TEST(AllocateBuffer, Byte_size_1500_ExternalArg) {
	{
		LockFreeRingBuffer<uint8_t, MockAllocator<uint8_t>> ringBuffer(Size());
		EXPECT_EQ(ringBuffer.capacity(), 2047);
	}

	EXPECT_EQ(constructorCallCounter, 1);
	EXPECT_EQ(destructorCallCounter, 1);
}

}

namespace Test_AllocateBuffer_UniquePtrInt_size_0 {

int32_t constructorCallCounter = 0;
int32_t destructorCallCounter = 0;

template<class _Ty>
struct MockAllocator {
	static _Ty* Allocate(size_t size) noexcept {
		constructorCallCounter++;
		return (reinterpret_cast<_Ty*>(0xF0));
	}

	static void DeAllocate(_Ty* const ptr) noexcept {
		EXPECT_EQ(ptr, nullptr);
		destructorCallCounter++;
	}
};

TEST(AllocateBuffer, UniquePtrInt_size_0) {
	{
		LockFreeRingBuffer<std::unique_ptr<int>, MockAllocator<std::unique_ptr<int>>> ringBuffer(0);
		EXPECT_EQ(ringBuffer.capacity(), 0);
	}

	EXPECT_EQ(constructorCallCounter, 0);
	EXPECT_EQ(destructorCallCounter, 1);
}

}

namespace Test_AllocateBuffer_UniquePtrInt_size_10 {

int32_t constructorCallCounter = 0;
int32_t destructorCallCounter = 0;

template<class _Ty>
struct MockAllocator {
	static _Ty* Allocate(size_t size) noexcept {
		constructorCallCounter++;
		EXPECT_EQ(size, 16);
		return (reinterpret_cast<_Ty*>(0xF0));
	}

	static void DeAllocate(_Ty* const ptr) noexcept {
		destructorCallCounter++;
		EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr), 0xF0);
	}
};

TEST(AllocateBuffer, UniquePtrInt_size_10) {
	{
		LockFreeRingBuffer<std::unique_ptr<int>, MockAllocator<std::unique_ptr<int>>> ringBuffer(10);
		EXPECT_EQ(ringBuffer.capacity(), 15);
	}

	EXPECT_EQ(constructorCallCounter, 1);
	EXPECT_EQ(destructorCallCounter, 1);
}

}