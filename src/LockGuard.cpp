#include <iostream>
#include <thread>
#include <mutex>

long long counter = 0;
std::mutex mtx;

void increment() {
    for (int i = 0; i < 100000; ++i) {
        // The lock is acquired when 'guard' is created.
        std::lock_guard<std::mutex> guard(mtx);

        // The lock is automatically released when 'guard' goes out of scope
        // at the end of each loop iteration.
        counter++;
    } // <- guard is destroyed here, mtx is unlocked.
}

int main() {
    std::thread t1(increment);
    std::thread t2(increment);

    t1.join();
    t2.join();

    std::cout << "Final counter value: " << counter << std::endl;
    return 0;
}