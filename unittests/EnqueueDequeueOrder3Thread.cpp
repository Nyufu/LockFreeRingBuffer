#include "pch.h"

#pragma warning( push, 0 )

#include "gtest\gtest.h"
#include "gmock\gmock.h"
#include <atomic>
#include <memory>
#include <thread>

#pragma warning( pop )

#include "LockFreeRingBuffer.h"
#include "Utility.h"

TEST(EnqueueDequeueOrder3Thread, Int_2Produces1ConsumerThreads) {
	LockFreeRingBuffer<int> ringBuffer(15000);

	std::vector<int> outputData;
	std::atomic_int event_(0);
	std::atomic_bool signal(true);

	outputData.reserve(65536);

	std::thread producer0([&] {
		event_++;
		while (event_.load() != 3)
			;

		for (auto i : intInputData_Size32768_0)
			while (!ringBuffer.enqueue(i))
				;

		signal.store(false);
	});

	std::thread producer1([&] {
		event_++;
		while (event_.load() != 3)
			;

		for (auto i : intInputData_Size32768_1)
			while (!ringBuffer.enqueue(i))
				;

		signal.store(false);
	});

	std::thread consumer([&] {
		event_++;
		while (event_.load() != 3)
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

	if (!producer0.joinable() || !producer1.joinable() || !consumer.joinable())
		FAIL();

	producer0.join();
	producer1.join();
	consumer.join();

	auto iterResult = outputData.cbegin();
	for (auto iter0 = intInputData_Size32768_0.cbegin(), iter1 = intInputData_Size32768_1.cbegin();
		iter0 == intInputData_Size32768_0.cend() && iter1 == intInputData_Size32768_1.cend();
		) {
		if (*iterResult == *iter0) {
			iterResult++;
			iter0++;
			continue;
		}

		if (*iterResult == *iter1) {
			iterResult++;
			iter1++;
			continue;
		}

		FAIL();
	}
}

TEST(EnqueueDequeueOrder3Thread, Int_1Produces2ConsumerThreads) {
	LockFreeRingBuffer<int> ringBuffer(7000);

	std::vector<int> outputData0;
	std::vector<int> outputData1;
	std::atomic_int event_(0);
	std::atomic_bool signal(true);

	outputData0.reserve(32768);
	outputData1.reserve(32768);

	std::thread producer([&] {
		event_++;
		while (event_.load() != 3)
			;

		for (auto i : intInputData_Size32768_0)
			while (!ringBuffer.enqueue(i))
				;

		signal.store(false);
	});

	std::thread consumer0([&] {
		event_++;
		while (event_.load() != 3)
			;

		bool finished = false;
		int value = 0;

		while (signal.load() || !finished) {
			finished = true;
			if (ringBuffer.dequeue(value)) {
				outputData0.push_back(value);
				finished = false;
			}
		}
	});

	std::thread consumer1([&] {
		event_++;
		while (event_.load() != 3)
			;

		bool finished = false;
		int value = 0;

		while (signal.load() || !finished) {
			finished = true;
			if (ringBuffer.dequeue(value)) {
				outputData1.push_back(value);
				finished = false;
			}
		}
	});

	if (!producer.joinable() || !consumer0.joinable() || !consumer1.joinable())
		FAIL();

	producer.join();
	consumer0.join();
	consumer1.join();

	auto iterInput = intInputData_Size32768_0.cbegin();
	for (auto iter0 = outputData0.cbegin(), iter1 = outputData1.cbegin();
		iter0 == outputData0.cend() && iter1 == outputData1.cend();
		) {
		if (*iterInput == *iter0) {
			iterInput++;
			iter0++;
			continue;
		}

		if (*iterInput == *iter1) {
			iterInput++;
			iter1++;
			continue;
		}

		FAIL();
	}
}