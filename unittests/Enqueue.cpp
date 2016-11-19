#pragma warning( push, 0 )

#include "gtest\gtest.h"
#include <memory>

#pragma warning( pop )

#include "LockFreeRingBuffer.h"

TEST(Enqueue, Int_size_0) {
	LockFreeRingBuffer<int> ringBuffer(0);
	EXPECT_FALSE(ringBuffer.enqueue(1));
}

TEST(Enqueue, Int_size_1) {
	LockFreeRingBuffer<int> ringBuffer(1);
	EXPECT_TRUE(ringBuffer.enqueue(1));
}

TEST(Enqueue, Int_size_1_enqueue_2) {
	LockFreeRingBuffer<int> ringBuffer(1);
	EXPECT_TRUE(ringBuffer.enqueue(1));
	EXPECT_FALSE(ringBuffer.enqueue(2));
}

TEST(Enqueue, Int_size_2) {
	LockFreeRingBuffer<int> ringBuffer(2);
	EXPECT_TRUE(ringBuffer.enqueue(1));
	EXPECT_TRUE(ringBuffer.enqueue(2));
}

TEST(Enqueue, Int_size_2_same_value) {
	LockFreeRingBuffer<int> ringBuffer(2);
	EXPECT_TRUE(ringBuffer.enqueue(1));
	EXPECT_TRUE(ringBuffer.enqueue(1));
}

TEST(Enqueue, Int_size_16) {
	LockFreeRingBuffer<int> ringBuffer(16);

	for (int i = 0; i < 31; i++)
		EXPECT_TRUE(ringBuffer.enqueue(i));

	EXPECT_FALSE(ringBuffer.enqueue(32));
}