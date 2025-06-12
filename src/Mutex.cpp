#include <iostream>
#include <thread>
#include <mutex> // Include the mutex header

long long counter = 0;
std::mutex mtx; // Create a mutex object

void increment() {
    for (int i = 0; i < 100000; ++i) {
        mtx.lock(); // Acquire the lock
        int temp = counter; // 1. Read
        temp++;                   // 2. Modify
        counter = temp;           // 3. Write
        mtx.unlock(); // Release the lock
    }
}

int main() {
    std::thread t1(increment);
    std::thread t2(increment);

    t1.join();
    t2.join();

    // Now, this will always print 200000.
    std::cout << "Final counter value: " << counter << std::endl;

    return 0;
}