#pragma warning( push, 0 )

#include "gtest\gtest.h"
#include "gmock\gmock.h"
#include <atomic>
#include <memory>
#include <thread>

#pragma warning( pop )

#include "LockFreeRingBuffer.h"
#include "Utility.h"

TEST(EnqueueDequeueOrder4Thread, Int_2Produces2ConsumerThreads) {
	LockFreeRingBuffer<int> ringBuffer(15000);

	std::vector<int> outputData0;
	std::vector<int> outputData1;
	std::atomic_int event_(0);
	std::atomic_bool signal(true);

	outputData0.reserve(65536);
	outputData1.reserve(65536);

	std::thread producer0([&] {
		event_++;
		while (event_.load() != 4)
			;

		for (auto i : intInputData_Size32768_0)
			while (!ringBuffer.enqueue(i))
				;

		signal.store(false);
	});

	std::thread producer1([&] {
		event_++;
		while (event_.load() != 4)
			;

		for (auto i : intInputData_Size32768_1)
			while (!ringBuffer.enqueue(i))
				;

		signal.store(false);
	});

	std::thread consumer0([&] {
		event_++;
		while (event_.load() != 4)
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
		while (event_.load() != 4)
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

	if (!producer0.joinable() || !producer1.joinable() || !consumer0.joinable() || !consumer1.joinable())
		FAIL();

	producer0.join();
	producer1.join();
	consumer0.join();
	consumer1.join();

	for (auto iterInput0 = intInputData_Size32768_0.cbegin(), iterInput1 = intInputData_Size32768_1.cbegin(),
		iterResult0 = outputData0.cbegin(), iterResult1 = outputData1.cbegin();
		iterInput0 == intInputData_Size32768_0.cend() && iterInput1 == intInputData_Size32768_1.cend() &&
		iterResult0 == outputData0.cend() && iterResult1 == outputData1.cend();
		) {
		if (*iterInput0 == *iterResult0) {
			iterInput0++;
			iterResult0++;
			continue;
		}

		if (*iterInput0 == *iterResult1) {
			iterInput0++;
			iterResult1++;
			continue;
		}

		if (*iterInput1 == *iterResult0) {
			iterInput1++;
			iterResult0++;
			continue;
		}

		if (*iterInput1 == *iterResult1) {
			iterInput1++;
			iterResult1++;
			continue;
		}

		FAIL();
	}
}