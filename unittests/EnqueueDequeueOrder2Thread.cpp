#pragma warning( push, 0 )

#include "gtest\gtest.h"
#include "gmock\gmock.h"
#include <atomic>
#include <memory>
#include <thread>

#pragma warning( pop )

#include "LockFreeRingBuffer.h"
#include "Utility.h"

TEST(EnqueueDequeueOrder2Thread, Int_1Produces1ConsumerThreads) {
	LockFreeRingBuffer<int> ringBuffer(15);

	std::vector<int> outputData;
	std::atomic_int event_(0);
	std::atomic_bool signal(true);

	std::thread producer([&] {
		event_++;
		while (event_.load() != 2)
			;

		for (auto i : intInputData_Size33)
			while (!ringBuffer.enqueue(i))
				;

		signal.store(false);
	});

	std::thread consumer([&] {
		event_++;
		while (event_.load() != 2)
			;

		bool finished = false;
		int value = 0;

		while (signal.load() || !finished) {
			finished = true;
			if (ringBuffer.dequeue(value)) {
				outputData.push_back(value);
				finished = false;
			}
		}
	});

	if (!producer.joinable() || !consumer.joinable())
		FAIL();

	producer.join();
	consumer.join();

	EXPECT_THAT(intInputData_Size33, ::testing::ContainerEq(outputData));
}

TEST(EnqueueDequeueOrder2Thread, SharedPtrInt_1Produces1ConsumerThreads) {
	LockFreeRingBuffer<std::shared_ptr<int>> ringBuffer(15);

	std::vector<std::shared_ptr<int>> inputData;
	std::vector<std::shared_ptr<int>> outputData;
	std::atomic_int event_(0);
	std::atomic_bool signal(true);

	for (auto i : intInputData_Size33)
		inputData.push_back(std::make_shared<int>(i));

	std::thread producer([&] {
		event_++;
		while (event_.load() != 2)
			;

		for (auto i : inputData)
			while (!ringBuffer.enqueue(i))
				;

		signal.store(false);
	});

	std::thread consumer([&] {
		event_++;
		while (event_.load() != 2)
			;

		bool finished = false;
		std::shared_ptr<int> value = 0;

		while (signal.load() || !finished) {
			finished = true;
			if (ringBuffer.dequeue(value)) {
				outputData.push_back(value);
				finished = false;
			}
		}
	});

	if (!producer.joinable() || !consumer.joinable())
		FAIL();

	producer.join();
	consumer.join();

	EXPECT_THAT(inputData, ::testing::ContainerEq(outputData));
}