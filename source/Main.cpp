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

#pragma warning( pop )

#define _TEST 1
#include "LockFreeRingBuffer.h"

using namespace std::chrono_literals;

#pragma optimize( "", off )
__declspec( noinline ) void bla( int )
{
}
#pragma optimize( "", on )

constexpr size_t Number = 4;
constexpr auto timeLimit = 1000ms;

int main()
{
  LockFreeRingBuffer<int> ringBuffer{ 2000 };

  std::atomic_int64_t event_ = 0;
  std::atomic_bool runIt = true;

  std::atomic_int64_t counters[ Number ];
  for ( size_t i = 0; i < Number; i++ )
    counters[ i ].store( 0 );

  std::thread producer[ Number ] = {
    std::thread( [ &ringBuffer, &event_, &runIt, &counters ]( const int i ) {
      auto t = i + 1;

      event_++;
      while ( event_.load() != ( Number * 2 ) );

      while ( runIt )
        if ( ringBuffer.enqueue( t ) )
          counters[ i ]++;
    }, 0 ),

    std::thread( [ &ringBuffer, &event_, &runIt, &counters ]( const int i ) {
      auto t = i + 1;

      event_++;
      while ( event_.load() != ( Number * 2 ) );

      while ( runIt )
        if ( ringBuffer.enqueue( t ) )
          counters[ i ]++;
    }, 1 ),

    std::thread( [ &ringBuffer, &event_, &runIt, &counters ]( const int i ) {
      auto t = i + 1;

      event_++;
      while ( event_.load() != ( Number * 2 ) );

      while ( runIt )
        if ( ringBuffer.enqueue( t ) )
          counters[ i ]++;
    }, 2 ),

    std::thread( [ &ringBuffer, &event_, &runIt, &counters ]( const int i ) {
      auto t = i + 1;

      event_++;
      while ( event_.load() != ( Number * 2 ) );

      while ( runIt )
        if ( ringBuffer.enqueue( t ) )
          counters[ i ]++;
    }, 3 )
  };

  std::thread consumer[ Number ] = {
    std::thread( [ &ringBuffer, &event_, &runIt, &counters ]() {
      int value = 0;

      event_++;
      while ( event_.load() != ( Number * 2 ) );

      while ( runIt )
        if ( ringBuffer.try_dequeue( value ) )
          counters[ value - 1 ]--;
    } ),

    std::thread( [ &ringBuffer, &event_, &runIt, &counters ]() {
      int value = 0;

      event_++;
      while ( event_.load() != ( Number * 2 ) );

      while ( runIt )
        if ( ringBuffer.try_dequeue( value ) )
          counters[ value - 1 ]--;
    } ),

    std::thread( [ &ringBuffer, &event_, &runIt, &counters ]() {
      int value = 0;

      event_++;
      while ( event_.load() != ( Number * 2 ) );

      while ( runIt )
        if ( ringBuffer.try_dequeue( value ) )
          counters[ value - 1 ]--;
    } ),

    std::thread( [ &ringBuffer, &event_, &runIt, &counters ]() {
      int value = 0;

      event_++;
      while ( event_.load() != ( Number * 2 ) );

      while ( runIt )
        if ( ringBuffer.try_dequeue( value ) )
          counters[ value - 1 ]--;
    } )
  };

  while ( event_.load() != ( Number * 2 ) );

  std::this_thread::sleep_for( timeLimit );

  runIt.store( false );

  for ( size_t i = 0; i < Number; i++ )
  {
    producer[ i ].join();
    consumer[ i ].join();
  }

  {
    int value = 0;

    while ( ringBuffer.try_dequeue( value ) )
      counters[ value - 1 ]--;
  }

  auto remainedInQueue = ringBuffer.size_approx();

  int64_t summary = 0;
  for ( size_t i = 0; i < Number; i++ )
    summary += counters[ i ];

  std::cout << "remainedInQueue: " << remainedInQueue << "\n";
  std::cout << "Counters summary: " << summary << "\n";

#if (defined (_TEST) && _TEST)

  std::cout << "number of iops: " << ringBuffer.max() << "\n";

#endif

  return 0;
}