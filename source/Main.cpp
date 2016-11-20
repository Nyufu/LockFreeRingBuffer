// MIT License
//
// Copyright (c) 2016 Andris Nyiscsák
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma warning( push, 0 )

#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <intrin.h>

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>

#pragma warning( pop )

#include "LockFreeRingBuffer.h"

using namespace std::chrono_literals;

constexpr size_t Number = 2;
constexpr auto timeLimit = 1000ms;

void Test1();
void Test2();

int main() {
	Test1();
	Test2();

	return 0;
}

void Test1() {
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

		//std::thread([&ringBuffer, &event_, &runIt, &counterProducer](const int i) {
		//	event_++;
		//	while (event_.load() != (Number * 2))
		//		;
		//
		//	int64_t counter = 0;
		//
		//	while (runIt)
		//		if (ringBuffer.enqueue(i))
		//			counter++;
		//
		//	counterProducer.fetch_add(counter);
		//}, 3),
		//
		//std::thread([&ringBuffer, &event_, &runIt, &counterProducer](const int i) {
		//	event_++;
		//	while (event_.load() != (Number * 2))
		//		;
		//
		//	int64_t counter = 0;
		//
		//	while (runIt)
		//		if (ringBuffer.enqueue(i))
		//			counter++;
		//
		//	counterProducer.fetch_add(counter);
		//}, 4)
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

		//std::thread([&event_, &runIt, &ringBuffer, &counterConsumer]() {
		//	int value = 0;
		//	int64_t localConsumerCounter[Number + 1];
		//	memset(&localConsumerCounter, 0, sizeof(int64_t) * (Number + 1));
		//
		//	event_++;
		//	while (event_.load() != (Number * 2))
		//		;
		//
		//	while (runIt)
		//		if (ringBuffer.dequeue(value))
		//			localConsumerCounter[value]++;
		//
		//	for (size_t i = 0; i < Number; i++)
		//		counterConsumer.fetch_add(localConsumerCounter[i + 1]);
		//}),
		//
		//std::thread([&event_, &runIt, &ringBuffer, &counterConsumer]() {
		//	int value = 0;
		//	int64_t localConsumerCounter[Number + 1];
		//	memset(&localConsumerCounter, 0, sizeof(int64_t) * (Number + 1));
		//
		//	event_++;
		//	while (event_.load() != (Number * 2))
		//		;
		//
		//	while (runIt)
		//		if (ringBuffer.dequeue(value))
		//			localConsumerCounter[value]++;
		//
		//	for (size_t i = 0; i < Number; i++)
		//		counterConsumer.fetch_add(localConsumerCounter[i + 1]);
		//})
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

	auto remainedInQueue = ringBuffer.size_approx();

	std::cout << "remainedInQueue: " << remainedInQueue << "\n";
	std::cout << "Counters summary: " << counterProducer.load() - counterConsumer.load() << "\n";
	std::cout << "number of iops: " << counterProducer.load() << "\n";
}

void Test2() {
	LockFreeRingBuffer<std::vector<int>> ringBuffer{ 2000 };
	ringBuffer.enqueue(std::vector<int>{1});

	std::vector<int> vec;
	ringBuffer.dequeue(vec);
	ringBuffer.dequeue(vec);
	ringBuffer.enqueue(vec);
}