// You MUST Worry with Atomics: 
// If you are using std::atomic to synchronize access to other non-atomic data (like our flag example), 
// you must consider the memory model.
// Default is Your Friend:
// Stick to the default std::memory_order_seq_cst. Only deviate from it if you are an expert,
// you have profiled your application and found a significant bottleneck on a specific atomic operation,
// and you can mathematically prove that a more relaxed ordering is correct.
// Incorrect use of relaxed atomics is a source of horrifying, hard-to-find bugs.
#include <iostream>
#include <thread>
#include <vector>
#include <atomic> // Include the atomic header

// Wrap the counter in std::atomic.
std::atomic<long long> atomic_counter = {0};

void increment() {
    for (int i = 0; i < 100000; ++i) {
        // This increment operation is now atomic.
        // It's a single, indivisible hardware instruction.
        atomic_counter++;
    }
}

int main() {
    std::thread t1(increment);
    std::thread t2(increment);

    t1.join();
    t2.join();

    // The result is now guaranteed to be 200000.
    // The read operation is also atomic.
    std::cout << "Final counter value: " << atomic_counter << std::endl;

    return 0;
}