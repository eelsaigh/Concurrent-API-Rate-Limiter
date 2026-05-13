# Concurrent API Rate Limiter (Token Bucket)

A high-performance, multithreaded API rate limiter built in C. This project simulates concurrent incoming API requests and manages them using a strict Token Bucket algorithm to prevent server overload.

Unlike polling-based rate limiters that waste CPU cycles, this implementation utilizes a **lazy refill algorithm**, calculating token replenishment dynamically based on microsecond time differentials only when a request is made.

## Key Technical Concepts Demonstrated

* **Concurrency & Pthreads:** Simulates multiple simultaneous network requests using POSIX threads (`pthread_create`, `pthread_join`).
* **Thread Safety & Synchronization:** Utilizes `pthread_mutex_t` to lock the shared `Bucket` state, strictly preventing race conditions when multiple threads attempt to consume tokens simultaneously.
* **Lazy Refill Algorithm:** Uses `<sys/time.h>` to calculate time elapsed since the last request, adding tokens proportionally to the defined rate before evaluating the current request.
* **Dynamic Memory Management:** Safely handles struct allocation and deallocation for incoming request payloads.

## System Architecture

The core of the system relies on the `allow()` function, which acts as the thread-safe gatekeeper:
1. Acquires the Mutex lock.
2. Triggers the `refill()` calculation based on system time.
3. Evaluates if tokens are >= 1.0.
4. Deducts the token (if approved) and releases the lock.
5. Returns a boolean simulating an HTTP 200 (OK) or HTTP 429 (Too Many Requests).

## How to Compile and Run

This project relies on the POSIX thread library. Compile it using `gcc` with the `-lpthread` flag.

```bash
# Compile the C file
gcc "Rate Limiter.c" -o limiter -lpthread

# Execute the compiled binary
./limiter 
