#pragma warning( push, 0 )

#include "gtest\gtest.h"
#include <memory>

#pragma warning( pop )

#include "LockFreeRingBuffer.h"

namespace Test_AllocateBuffer_Byte_1_size {

int32_t constructorCallCounter = 0;
int32_t destructorCallCounter = 0;

template<class _Ty>
struct MockAllocator {
  static _Ty* Allocate(size_t size) noexcept {
    constructorCallCounter++;
    EXPECT_EQ( size, 2 );
    return ( reinterpret_cast<_Ty*>( 0xF0 ) );
  }

  static void DeAllocate(_Ty* const ptr) noexcept {
    destructorCallCounter++;
    EXPECT_EQ( reinterpret_cast<uintptr_t>( ptr ), 0xF0 );
  }
};

TEST(AllocateBuffer, Byte_1_size) {
  {
	  LockFreeRingBuffer<uint8_t, MockAllocator<uint8_t>> ringBuffer(1);
	  EXPECT_EQ(ringBuffer.capacity(), 1);
  }

	EXPECT_EQ(constructorCallCounter, 1);
	EXPECT_EQ(destructorCallCounter, 1);
}

}

namespace Test_AllocateBuffer_Byte_1000_size {

int32_t constructorCallCounter = 0;
int32_t destructorCallCounter = 0;

template<class _Ty>
struct MockAllocator {
  static _Ty* Allocate(size_t size) noexcept {
    constructorCallCounter++;
    EXPECT_EQ( size, 1024 );
    return ( reinterpret_cast<_Ty*>( 0xF0 ) );
  }

  static void DeAllocate(_Ty* const ptr) noexcept {
    destructorCallCounter++;
    EXPECT_EQ( reinterpret_cast<uintptr_t>( ptr ), 0xF0 );
  }
};

TEST(AllocateBuffer, Byte_1000_size) {
  {
	  LockFreeRingBuffer<uint8_t, MockAllocator<uint8_t>> ringBuffer(1000);
	  EXPECT_EQ(ringBuffer.capacity(), 1023);
  }

	EXPECT_EQ(constructorCallCounter, 1);
	EXPECT_EQ(destructorCallCounter, 1);
}

}

namespace Test_AllocateBuffer_UniquePtrInt_10_size {

int32_t constructorCallCounter = 0;
int32_t destructorCallCounter = 0;

template<class _Ty>
struct MockAllocator {
  static _Ty* Allocate(size_t size) noexcept {
    constructorCallCounter++;
    EXPECT_EQ( size, 16 );
    return ( reinterpret_cast<_Ty*>( 0xF0 ) );
  }

  static void DeAllocate(_Ty* const ptr) noexcept {
    destructorCallCounter++;
    EXPECT_EQ( reinterpret_cast<uintptr_t>( ptr ), 0xF0 );
  }
};

TEST(AllocateBuffer, UniquePtrInt_10_size) {
  {
	  LockFreeRingBuffer<std::unique_ptr<int>, MockAllocator<std::unique_ptr<int>>> ringBuffer(10);
	  EXPECT_EQ(ringBuffer.capacity(), 15);
  }

	EXPECT_EQ(constructorCallCounter, 1);
	EXPECT_EQ(destructorCallCounter, 1);
}

}