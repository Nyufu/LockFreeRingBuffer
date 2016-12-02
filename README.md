# LockFreeRingBuffer
Multiple producer multiple consumer lockfree ringbuffer.

## Features
- Compliance to the C++11 standards.
- Fully portable (all is done through standard C++11 primitives).
- Fully thread-safe lock-free ringbuffer. Use concurrently from any number of threads.
- Only the constructor can throw exceptions.
- Header-only implementation with separated allocator.
