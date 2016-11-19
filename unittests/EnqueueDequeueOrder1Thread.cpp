#pragma warning( push, 0 )

#include "gtest\gtest.h"
#include "gmock\gmock.h"
#include <memory>

#pragma warning( pop )

#include "LockFreeRingBuffer.h"
#include "Utility.h"

TEST(EnqueueDequeueOrder1Thread, Int_MoreEnqueue) {
	constexpr auto batchNumber = 5;
	LockFreeRingBuffer<int> ringBuffer(15);

	std::vector<int> outputData;

	for (auto iter = intInputData.cbegin(); ; ) {
		while (iter != intInputData.cend() && ringBuffer.enqueue(*iter))
			iter++;

		for (int j = 0; j < batchNumber; j++) {
			int value = 0;
			if (ringBuffer.dequeue(value))
				outputData.push_back(value);
		}

		if (iter == intInputData.cend() && ringBuffer.size_approx() == 0)
			break;
	}

	EXPECT_THAT(intInputData, ::testing::ContainerEq(outputData));
}

TEST(EnqueueDequeueOrder1Thread, IntRef_MoreEnqueue) {
	constexpr auto batchNumber = 5;
	LockFreeRingBuffer<int> ringBuffer(15);

	std::vector<int> outputData;

	for (auto iter = intInputData.cbegin(); ; ) {
		while (iter != intInputData.cend()) {
			auto& ref = *iter;
			if (ringBuffer.enqueue(ref))
				iter++;
			else
				break;
		}

		for (int j = 0; j < batchNumber; j++) {
			int value = 0;
			if (ringBuffer.dequeue(value))
				outputData.push_back(value);
		}

		if (iter == intInputData.cend() && ringBuffer.size_approx() == 0)
			break;
	}

	EXPECT_THAT(intInputData, ::testing::ContainerEq(outputData));
}

TEST(EnqueueDequeueOrder1Thread, Int_MoreDequeue) {
	constexpr auto batchNumber = 5;
	LockFreeRingBuffer<int> ringBuffer(15);

	std::vector<int> outputData;

	for (auto iter = intInputData.cbegin(); ; ) {

		for (int j = 0; j < batchNumber; j++)
			if (iter != intInputData.cend() && ringBuffer.enqueue(*iter))
				iter++;

		int value = 0;
		while (ringBuffer.dequeue(value))
			outputData.push_back(value);

		if (iter == intInputData.cend())
			break;
	}

	EXPECT_THAT(intInputData, ::testing::ContainerEq(outputData));
}

TEST(EnqueueDequeueOrder1Thread, IntRef_MoreDequeue) {
	constexpr auto batchNumber = 5;
	LockFreeRingBuffer<int> ringBuffer(15);

	std::vector<int> outputData;

	for (auto iter = intInputData.cbegin(); ; ) {

		for (int j = 0; j < batchNumber; j++)
			if (iter != intInputData.cend()) {
				auto& ref = *iter;
				if (ringBuffer.enqueue(ref))
					iter++;
			}

		int value = 0;
		while (ringBuffer.dequeue(value))
			outputData.push_back(value);

		if (iter == intInputData.cend())
			break;
	}

	EXPECT_THAT(intInputData, ::testing::ContainerEq(outputData));
}

TEST(EnqueueDequeueOrder1Thread, SharedPtrInt_MoreEnqueue) {
	constexpr auto batchNumber = 5;
	LockFreeRingBuffer<std::shared_ptr<int>> ringBuffer(15);

	std::vector<std::shared_ptr<int>> inputData;
	std::vector<std::shared_ptr<int>> outputData;

	for (auto i : intInputData)
		inputData.push_back(std::make_shared<int>(i));

	for (auto iter = inputData.begin(); ; ) {
		while (iter != inputData.cend() && ringBuffer.enqueue(*iter))
			iter++;

		for (int j = 0; j < batchNumber; j++) {
			std::shared_ptr<int> value = 0;
			if (ringBuffer.dequeue(value))
				outputData.push_back(value);
		}

		if (iter == inputData.cend() && ringBuffer.size_approx() == 0)
			break;
	}

	EXPECT_THAT(inputData, ::testing::ContainerEq(outputData));
}

TEST(EnqueueDequeueOrder1Thread, SharedPtrInt_MoreDequeue) {
	constexpr auto batchNumber = 5;
	LockFreeRingBuffer<std::shared_ptr<int>> ringBuffer(15);

	std::vector<std::shared_ptr<int>> inputData;
	std::vector<std::shared_ptr<int>> outputData;

	for (auto i : intInputData)
		inputData.push_back(std::make_shared<int>(i));

	for (auto iter = inputData.begin(); ; ) {
		for (int j = 0; j < batchNumber; j++)
			if (iter != inputData.end() && ringBuffer.enqueue(*iter))
				iter++;

		std::shared_ptr<int> value;
		while (ringBuffer.dequeue(value))
			outputData.push_back(value);

		if (iter == inputData.end())
			break;
	}

	EXPECT_THAT(inputData, ::testing::ContainerEq(outputData));
}