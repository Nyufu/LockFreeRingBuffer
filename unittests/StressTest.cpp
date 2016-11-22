#include "pch.h"

#pragma warning( push, 0 )

#include "gtest\gtest.h"
#include "gmock\gmock.h"
#include <intrin.h>
#include <chrono>
#include <thread>

#pragma warning( pop )

#include "LockFreeRingBuffer.h"

using namespace std::chrono_literals;

constexpr size_t Number = 4;
constexpr auto timeLimit = 1000ms;

TEST(StressTest, Int_4Produces4ConsumerThreads_1Sec) {
	LockFreeRingBuffer<int> ringBuffer{ 2000 };

	std::atomic_int64_t event_(0ll);
	std::atomic_bool runIt(true);

	int64_t countersProducer[Number + 1];
	memset(&countersProducer, 0, sizeof(int64_t) * (Number + 1));

	std::atomic_int64_t counterProducer;
	std::atomic_int64_t counterConsumer;
	counterProducer.store(0);
	counterConsumer.store(0);

	std::thread producer[Number] = {
		std::thread([&ringBuffer, &event_, &runIt, &counterProducer](const int i) {
			event_++;
			while (event_.load() != (Number * 2))
				;

			int64_t counter = 0;

			while (runIt)
				if (ringBuffer.enqueue(i))
					counter++;

			counterProducer.fetch_add(counter);
		}, 1),

		std::thread([&ringBuffer, &event_, &runIt, &counterProducer](const int i) {
			event_++;
			while (event_.load() != (Number * 2))
				;

			int64_t counter = 0;

			while (runIt)
				if (ringBuffer.enqueue(i))
					counter++;

			counterProducer.fetch_add(counter);
		}, 2),

		std::thread([&ringBuffer, &event_, &runIt, &counterProducer](const int i) {
			event_++;
			while (event_.load() != (Number * 2))
				;

			int64_t counter = 0;

			while (runIt)
				if (ringBuffer.enqueue(i))
					counter++;

			counterProducer.fetch_add(counter);
		}, 3),

		std::thread([&ringBuffer, &event_, &runIt, &counterProducer](const int i) {
			event_++;
			while (event_.load() != (Number * 2))
				;

			int64_t counter = 0;

			while (runIt)
				if (ringBuffer.enqueue(i))
					counter++;

			counterProducer.fetch_add(counter);
		}, 4)
	};

	std::thread consumer[Number] = {
		std::thread([&event_, &runIt, &ringBuffer, &counterConsumer]() {
			int value = 0;
			int64_t localConsumerCounter[Number + 1];
			memset(&localConsumerCounter, 0, sizeof(int64_t) * (Number + 1));

			event_++;
			while (event_.load() != (Number * 2))
				;

			while (runIt)
				if (ringBuffer.dequeue(value))
					localConsumerCounter[value]++;

			for (size_t i = 0; i < Number; i++)
				counterConsumer.fetch_add(localConsumerCounter[i + 1]);
		}),

		std::thread([&event_, &runIt, &ringBuffer, &counterConsumer]() {
			int value = 0;
			int64_t localConsumerCounter[Number + 1];
			memset(&localConsumerCounter, 0, sizeof(int64_t) * (Number + 1));

			event_++;
			while (event_.load() != (Number * 2))
				;

			while (runIt)
				if (ringBuffer.dequeue(value))
					localConsumerCounter[value]++;

			for (size_t i = 0; i < Number; i++)
				counterConsumer.fetch_add(localConsumerCounter[i + 1]);
		}),

		std::thread([&event_, &runIt, &ringBuffer, &counterConsumer]() {
			int value = 0;
			int64_t localConsumerCounter[Number + 1];
			memset(&localConsumerCounter, 0, sizeof(int64_t) * (Number + 1));

			event_++;
			while (event_.load() != (Number * 2))
				;

			while (runIt)
				if (ringBuffer.dequeue(value))
					localConsumerCounter[value]++;

			for (size_t i = 0; i < Number; i++)
				counterConsumer.fetch_add(localConsumerCounter[i + 1]);
		}),

		std::thread([&event_, &runIt, &ringBuffer, &counterConsumer]() {
			int value = 0;
			int64_t localConsumerCounter[Number + 1];
			memset(&localConsumerCounter, 0, sizeof(int64_t) * (Number + 1));

			event_++;
			while (event_.load() != (Number * 2))
				;

			while (runIt)
				if (ringBuffer.dequeue(value))
					localConsumerCounter[value]++;

			for (size_t i = 0; i < Number; i++)
				counterConsumer.fetch_add(localConsumerCounter[i + 1]);
		})
	};

	while (event_.load() != (Number * 2))
		;

	std::this_thread::sleep_for(timeLimit);

	runIt.store(false);

	for (size_t i = 0; i < Number; i++) {
		producer[i].join();
		consumer[i].join();
	}

	{
		int value = 0;

		while (ringBuffer.dequeue(value))
			counterConsumer.fetch_add(1);
	}

	EXPECT_EQ(counterProducer.load(), counterConsumer.load());

}