#pragma warning( push, 0 )

#include "gtest\gtest.h"
#include <memory>

#pragma warning( pop )

#include "LockFreeRingBuffer.h"

TEST(Dequeue, Int_size_0) {
	LockFreeRingBuffer<int> ringBuffer(0);
	int value = 0;
	EXPECT_FALSE(ringBuffer.dequeue(value));
}

TEST(Dequeue, Int_size_0_enqueue_2) {
	LockFreeRingBuffer<int> ringBuffer(0);
	int value = 0;
	EXPECT_FALSE(ringBuffer.dequeue(value));
	EXPECT_FALSE(ringBuffer.dequeue(value));
}

TEST(Dequeue, Int_size_1) {
	LockFreeRingBuffer<int> ringBuffer(1);
	ringBuffer.enqueue(1);

	int value = 0;
	EXPECT_TRUE(ringBuffer.dequeue(value));
	EXPECT_EQ(1, value);
}

TEST(Dequeue, Int_size_1_after_empty) {
	LockFreeRingBuffer<int> ringBuffer(1);
	ringBuffer.enqueue(1);

	int value = 0;
	EXPECT_TRUE(ringBuffer.dequeue(value));
	EXPECT_EQ(1, value);

	EXPECT_FALSE(ringBuffer.dequeue(value));
}

TEST(Dequeue, Int_size_1_after_empty2) {
	LockFreeRingBuffer<int> ringBuffer(1);
	ringBuffer.enqueue(1);

	int value = 0;
	EXPECT_TRUE(ringBuffer.dequeue(value));
	EXPECT_EQ(1, value);

	EXPECT_FALSE(ringBuffer.dequeue(value));

	ringBuffer.enqueue(5);
	EXPECT_TRUE(ringBuffer.dequeue(value));
	EXPECT_EQ(5, value);
}

TEST(Dequeue, Int_size_16) {
	LockFreeRingBuffer<int> ringBuffer(16);

	for (int i = 0; i < 31; i++)
		ringBuffer.enqueue(i);

	for (int i = 0; i < 31; i++) {
		int value = 0;
		EXPECT_TRUE(ringBuffer.dequeue(value));
		EXPECT_EQ(i, value);
	}
}

TEST(Dequeue, SharedPtrInt_size_0) {
	LockFreeRingBuffer<std::shared_ptr<int>> ringBuffer(0);
	std::shared_ptr<int> value = 0;
	EXPECT_FALSE(ringBuffer.dequeue(value));
}

TEST(Dequeue, SharedPtrInt_size_0_enqueue_2) {
	LockFreeRingBuffer<std::shared_ptr<int>> ringBuffer(0);
	std::shared_ptr<int> value = 0;
	EXPECT_FALSE(ringBuffer.dequeue(value));
	EXPECT_FALSE(ringBuffer.dequeue(value));
}

TEST(Dequeue, SharedPtrInt_size_1) {
	LockFreeRingBuffer<std::shared_ptr<int>> ringBuffer(1);
	ringBuffer.enqueue(std::make_shared<int>(1));

	std::shared_ptr<int> value = 0;
	EXPECT_TRUE(ringBuffer.dequeue(value));
	EXPECT_EQ(1, *value);
}

TEST(Dequeue, SharedPtrInt_size_1_after_empty) {
	LockFreeRingBuffer<std::shared_ptr<int>> ringBuffer(1);
	ringBuffer.enqueue(std::make_shared<int>(1));

	std::shared_ptr<int> value = 0;
	EXPECT_TRUE(ringBuffer.dequeue(value));
	EXPECT_EQ(1, *value);

	EXPECT_FALSE(ringBuffer.dequeue(value));
}

TEST(Dequeue, SharedPtrInt_size_1_after_empty2) {
	LockFreeRingBuffer<std::shared_ptr<int>> ringBuffer(1);
	ringBuffer.enqueue(std::make_shared<int>(1));

	std::shared_ptr<int> value = 0;
	EXPECT_TRUE(ringBuffer.dequeue(value));
	EXPECT_EQ(1, *value);

	EXPECT_FALSE(ringBuffer.dequeue(value));

	ringBuffer.enqueue(std::make_shared<int>(5));
	EXPECT_TRUE(ringBuffer.dequeue(value));
	EXPECT_EQ(5, *value);
}

TEST(Dequeue, SharedPtrInt_size_16) {
	LockFreeRingBuffer<std::shared_ptr<int>> ringBuffer(16);

	for (int i = 0; i < 31; i++)
		ringBuffer.enqueue(std::make_shared<int>(i));

	for (int i = 0; i < 31; i++) {
		std::shared_ptr<int> value = 0;
		EXPECT_TRUE(ringBuffer.dequeue(value));
		EXPECT_EQ(i, *value);
	}
}

//-----------------------------------------

TEST(Dequeue, UniquePtrInt_size_0) {
	LockFreeRingBuffer<std::unique_ptr<int>> ringBuffer(0);
	std::unique_ptr<int> value = 0;
	EXPECT_FALSE(ringBuffer.dequeue(value));
}

TEST(Dequeue, UniquePtrInt_size_0_enqueue_2) {
	LockFreeRingBuffer<std::unique_ptr<int>> ringBuffer(0);
	std::unique_ptr<int> value = 0;
	EXPECT_FALSE(ringBuffer.dequeue(value));
	EXPECT_FALSE(ringBuffer.dequeue(value));
}

TEST(Dequeue, UniquePtrInt_size_1) {
	LockFreeRingBuffer<std::unique_ptr<int>> ringBuffer(1);
	ringBuffer.enqueue(std::make_unique<int>(1));

	std::unique_ptr<int> value = 0;
	EXPECT_TRUE(ringBuffer.dequeue(value));
	EXPECT_EQ(1, *value);
}

TEST(Dequeue, UniquePtrInt_size_1_after_empty) {
	LockFreeRingBuffer<std::unique_ptr<int>> ringBuffer(1);
	ringBuffer.enqueue(std::make_unique<int>(1));

	std::unique_ptr<int> value = 0;
	EXPECT_TRUE(ringBuffer.dequeue(value));
	EXPECT_EQ(1, *value);

	EXPECT_FALSE(ringBuffer.dequeue(value));
}

TEST(Dequeue, UniquePtrInt_size_1_after_empty2) {
	LockFreeRingBuffer<std::unique_ptr<int>> ringBuffer(1);
	ringBuffer.enqueue(std::make_unique<int>(1));

	std::unique_ptr<int> value = 0;
	EXPECT_TRUE(ringBuffer.dequeue(value));
	EXPECT_EQ(1, *value);

	EXPECT_FALSE(ringBuffer.dequeue(value));

	ringBuffer.enqueue(std::make_unique<int>(5));
	EXPECT_TRUE(ringBuffer.dequeue(value));
	EXPECT_EQ(5, *value);
}

TEST(Dequeue, UniquePtrInt_size_16) {
	LockFreeRingBuffer<std::unique_ptr<int>> ringBuffer(16);

	for (int i = 0; i < 31; i++)
		ringBuffer.enqueue(std::make_unique<int>(i));

	for (int i = 0; i < 31; i++) {
		std::unique_ptr<int> value = 0;
		EXPECT_TRUE(ringBuffer.dequeue(value));
		EXPECT_EQ(i, *value);
	}
}