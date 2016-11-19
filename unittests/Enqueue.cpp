#pragma warning( push, 0 )

#include "gtest\gtest.h"
#include <memory>

#pragma warning( pop )

#include "LockFreeRingBuffer.h"

TEST(Enqueue, Int_size_0) {
	LockFreeRingBuffer<int> ringBuffer(0);
	EXPECT_FALSE(ringBuffer.enqueue(1));
}

TEST(Enqueue, Int_size_0_enqueue_2) {
	LockFreeRingBuffer<int> ringBuffer(0);
	EXPECT_FALSE(ringBuffer.enqueue(1));
	EXPECT_FALSE(ringBuffer.enqueue(2));
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

TEST(Enqueue, Int_enqueue_after_full) {
	LockFreeRingBuffer<int> ringBuffer(3);

	EXPECT_TRUE(ringBuffer.enqueue(1));
	EXPECT_TRUE(ringBuffer.enqueue(2));
	EXPECT_TRUE(ringBuffer.enqueue(3));

	EXPECT_FALSE(ringBuffer.enqueue(4));

	int value = 0;
	ringBuffer.dequeue(value);

	EXPECT_TRUE(ringBuffer.enqueue(5));
}

TEST(Enqueue, Int_enqueue_after_full2) {
	LockFreeRingBuffer<int> ringBuffer(3);

	EXPECT_TRUE(ringBuffer.enqueue(1));
	EXPECT_TRUE(ringBuffer.enqueue(2));
	EXPECT_TRUE(ringBuffer.enqueue(3));

	EXPECT_FALSE(ringBuffer.enqueue(4));

	int value = 0;
	ringBuffer.dequeue(value);

	EXPECT_TRUE(ringBuffer.enqueue(5));

	EXPECT_FALSE(ringBuffer.enqueue(6));
}

static_assert(sizeof(std::shared_ptr<int>) > sizeof(void*), "The std::shared_ptr<int> isn't enough large as expected.");

TEST(Enqueue, SharedPtrInt_size_1) {
	LockFreeRingBuffer<std::shared_ptr<int>> ringBuffer(1);
	EXPECT_TRUE(ringBuffer.enqueue(std::make_shared<int>(1)));
}

TEST(Enqueue, SharedPtrInt_size_1_enqueue_2) {
	LockFreeRingBuffer<std::shared_ptr<int>> ringBuffer(1);
	EXPECT_TRUE(ringBuffer.enqueue(std::make_shared<int>(1)));
	EXPECT_FALSE(ringBuffer.enqueue(std::make_shared<int>(2)));
}

TEST(Enqueue, SharedPtrInt_size_2) {
	LockFreeRingBuffer<std::shared_ptr<int>> ringBuffer(2);
	EXPECT_TRUE(ringBuffer.enqueue(std::make_shared<int>(1)));
	EXPECT_TRUE(ringBuffer.enqueue(std::make_shared<int>(2)));
}

TEST(Enqueue, SharedPtrInt_size_16) {
	LockFreeRingBuffer<std::shared_ptr<int>> ringBuffer(16);

	for (int i = 0; i < 31; i++)
		EXPECT_TRUE(ringBuffer.enqueue(std::make_shared<int>(i)));

	EXPECT_FALSE(ringBuffer.enqueue(std::make_shared<int>(32)));
}

TEST(Enqueue, SharedPtrInt_enqueue_after_full) {
	LockFreeRingBuffer<std::shared_ptr<int>> ringBuffer(3);

	EXPECT_TRUE(ringBuffer.enqueue(std::make_shared<int>(1)));
	EXPECT_TRUE(ringBuffer.enqueue(std::make_shared<int>(2)));
	EXPECT_TRUE(ringBuffer.enqueue(std::make_shared<int>(3)));

	EXPECT_FALSE(ringBuffer.enqueue(std::make_shared<int>(4)));

	std::shared_ptr<int> value = 0;
	ringBuffer.dequeue(value);

	EXPECT_TRUE(ringBuffer.enqueue(std::make_shared<int>(5)));
}

TEST(Enqueue, SharedPtrInt_enqueue_after_full2) {
	LockFreeRingBuffer<std::shared_ptr<int>> ringBuffer(3);

	EXPECT_TRUE(ringBuffer.enqueue(std::make_shared<int>(1)));
	EXPECT_TRUE(ringBuffer.enqueue(std::make_shared<int>(2)));
	EXPECT_TRUE(ringBuffer.enqueue(std::make_shared<int>(3)));

	EXPECT_FALSE(ringBuffer.enqueue(std::make_shared<int>(4)));

	std::shared_ptr<int> value = 0;
	ringBuffer.dequeue(value);

	EXPECT_TRUE(ringBuffer.enqueue(std::make_shared<int>(5)));

	EXPECT_FALSE(ringBuffer.enqueue(std::make_shared<int>(6)));
}

TEST(Enqueue, UniquePtrInt_size_1) {
	LockFreeRingBuffer<std::unique_ptr<int>> ringBuffer(1);
	EXPECT_TRUE(ringBuffer.enqueue(std::make_unique<int>(1)));
}

TEST(Enqueue, UniquePtrInt_size_1_enqueue_2) {
	LockFreeRingBuffer<std::unique_ptr<int>> ringBuffer(1);
	EXPECT_TRUE(ringBuffer.enqueue(std::make_unique<int>(1)));
	EXPECT_FALSE(ringBuffer.enqueue(std::make_unique<int>(2)));
}

TEST(Enqueue, UniquePtrInt_size_2) {
	LockFreeRingBuffer<std::unique_ptr<int>> ringBuffer(2);
	EXPECT_TRUE(ringBuffer.enqueue(std::make_unique<int>(1)));
	EXPECT_TRUE(ringBuffer.enqueue(std::make_unique<int>(2)));
}

TEST(Enqueue, UniquePtrInt_size_16) {
	LockFreeRingBuffer<std::unique_ptr<int>> ringBuffer(16);

	for (int i = 0; i < 31; i++)
		EXPECT_TRUE(ringBuffer.enqueue(std::make_unique<int>(i)));

	EXPECT_FALSE(ringBuffer.enqueue(std::make_unique<int>(32)));
}

TEST(Enqueue, UniquePtrInt_enqueue_after_full) {
	LockFreeRingBuffer<std::unique_ptr<int>> ringBuffer(3);

	EXPECT_TRUE(ringBuffer.enqueue(std::make_unique<int>(1)));
	EXPECT_TRUE(ringBuffer.enqueue(std::make_unique<int>(2)));
	EXPECT_TRUE(ringBuffer.enqueue(std::make_unique<int>(3)));

	EXPECT_FALSE(ringBuffer.enqueue(std::make_unique<int>(4)));

	std::unique_ptr<int> value = 0;
	ringBuffer.dequeue(value);

	EXPECT_TRUE(ringBuffer.enqueue(std::make_unique<int>(5)));
}

TEST(Enqueue, UniquePtrInt_enqueue_after_full2) {
	LockFreeRingBuffer<std::unique_ptr<int>> ringBuffer(3);

	EXPECT_TRUE(ringBuffer.enqueue(std::make_unique<int>(1)));
	EXPECT_TRUE(ringBuffer.enqueue(std::make_unique<int>(2)));
	EXPECT_TRUE(ringBuffer.enqueue(std::make_unique<int>(3)));

	EXPECT_FALSE(ringBuffer.enqueue(std::make_unique<int>(4)));

	std::unique_ptr<int> value = 0;
	ringBuffer.dequeue(value);

	EXPECT_TRUE(ringBuffer.enqueue(std::make_unique<int>(5)));

	EXPECT_FALSE(ringBuffer.enqueue(std::make_unique<int>(6)));
}