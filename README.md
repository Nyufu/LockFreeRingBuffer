# LockFreeRingBuffer
Multiple producer multiple consumer lockfree ringbuffer.

## Features
- Compliance to the C++11 standards.
- Fully portable (all is done through standard C++11 primitives).
- Fully thread-safe lock-free ringbuffer. Use concurrently from any number of threads.
- Header-only implementation with separated interchangeable allocator.
- Only the constructor allocates through the allocator.
- Only the allocator can throw exceptions from the constructor.
