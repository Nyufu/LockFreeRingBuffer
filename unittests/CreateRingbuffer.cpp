#pragma warning( push, 0 )

#include "gtest\gtest.h"
#include <memory>

#pragma warning( pop )

#include "LockFreeRingBuffer.h"

TEST(CreateRingbuffer, Int_0_size) {
	LockFreeRingBuffer<int> ringBuffer(0);
	EXPECT_EQ(ringBuffer.capacity(), 1);
	EXPECT_EQ(ringBuffer.size_approx(), 0);
}

TEST(CreateRingbuffer, Int_1_size) {
	LockFreeRingBuffer<int> ringBuffer(1);
	EXPECT_EQ(ringBuffer.capacity(), 1);
	EXPECT_EQ(ringBuffer.size_approx(), 0);
}

TEST(CreateRingbuffer, Int_1000_size) {
	LockFreeRingBuffer<int> ringBuffer(1000);
	EXPECT_EQ(ringBuffer.capacity(), 1023);
	EXPECT_EQ(ringBuffer.size_approx(), 0);
}

TEST(CreateRingbuffer, SharedPtrInt_0_size) {
	LockFreeRingBuffer<std::shared_ptr<int>> ringBuffer(0);
	EXPECT_EQ(ringBuffer.capacity(), 1);
	EXPECT_EQ(ringBuffer.size_approx(), 0);
}

TEST(CreateRingbuffer, SharedPtrInt_1_size) {
	LockFreeRingBuffer<std::shared_ptr<int>> ringBuffer(1);
	EXPECT_EQ(ringBuffer.capacity(), 1);
	EXPECT_EQ(ringBuffer.size_approx(), 0);
}

TEST(CreateRingbuffer, SharedPtrInt_1000_size) {
	LockFreeRingBuffer<std::shared_ptr<int>> ringBuffer(1000);
	EXPECT_EQ(ringBuffer.capacity(), 1023);
	EXPECT_EQ(ringBuffer.size_approx(), 0);
}

TEST(CreateRingbuffer, UniquePtrInt_0_size) {
	LockFreeRingBuffer<std::unique_ptr<int>> ringBuffer(0);
	EXPECT_EQ(ringBuffer.capacity(), 1);
	EXPECT_EQ(ringBuffer.size_approx(), 0);
}

TEST(CreateRingbuffer, UniquePtrInt_1_size) {
	LockFreeRingBuffer<std::unique_ptr<int>> ringBuffer(1);
	EXPECT_EQ(ringBuffer.capacity(), 1);
	EXPECT_EQ(ringBuffer.size_approx(), 0);
}

TEST(CreateRingbuffer, UniquePtrInt_1000_size) {
	LockFreeRingBuffer<std::unique_ptr<int>> ringBuffer(1000);
	EXPECT_EQ(ringBuffer.capacity(), 1023);
	EXPECT_EQ(ringBuffer.size_approx(), 0);
}