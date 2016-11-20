#pragma warning( push, 0 )

#include "gtest\gtest.h"
#include <memory>

#pragma warning( pop )

#include "LockFreeRingBuffer.h"

TEST(CreateRingbuffer, Int_size_0) {
	LockFreeRingBuffer<int> ringBuffer(0);
	EXPECT_EQ(ringBuffer.capacity(), 0);
	EXPECT_EQ(ringBuffer.size_approx(), 0);
}

TEST(CreateRingbuffer, Int_size_1) {
	LockFreeRingBuffer<int> ringBuffer(1);
	EXPECT_EQ(ringBuffer.capacity(), 1);
	EXPECT_EQ(ringBuffer.size_approx(), 0);
}

TEST(CreateRingbuffer, Int_size_1000) {
	LockFreeRingBuffer<int> ringBuffer(1000);
	EXPECT_EQ(ringBuffer.capacity(), 1023);
	EXPECT_EQ(ringBuffer.size_approx(), 0);
}

TEST(CreateRingbuffer, SharedPtrInt_size_0) {
	LockFreeRingBuffer<std::shared_ptr<int>> ringBuffer(0);
	EXPECT_EQ(ringBuffer.capacity(), 0);
	EXPECT_EQ(ringBuffer.size_approx(), 0);
}

TEST(CreateRingbuffer, SharedPtrInt_size_1) {
	LockFreeRingBuffer<std::shared_ptr<int>> ringBuffer(1);
	EXPECT_EQ(ringBuffer.capacity(), 1);
	EXPECT_EQ(ringBuffer.size_approx(), 0);
}

TEST(CreateRingbuffer, SharedPtrInt_size_1000) {
	LockFreeRingBuffer<std::shared_ptr<int>> ringBuffer(1000);
	EXPECT_EQ(ringBuffer.capacity(), 1023);
	EXPECT_EQ(ringBuffer.size_approx(), 0);
}

TEST(CreateRingbuffer, UniquePtrInt_size_0) {
	LockFreeRingBuffer<std::unique_ptr<int>> ringBuffer(0);
	EXPECT_EQ(ringBuffer.capacity(), 0);
	EXPECT_EQ(ringBuffer.size_approx(), 0);
}

TEST(CreateRingbuffer, UniquePtrInt_size_1) {
	LockFreeRingBuffer<std::unique_ptr<int>> ringBuffer(1);
	EXPECT_EQ(ringBuffer.capacity(), 1);
	EXPECT_EQ(ringBuffer.size_approx(), 0);
}

TEST(CreateRingbuffer, UniquePtrInt_size_1000) {
	LockFreeRingBuffer<std::unique_ptr<int>> ringBuffer(1000);
	EXPECT_EQ(ringBuffer.capacity(), 1023);
	EXPECT_EQ(ringBuffer.size_approx(), 0);
}