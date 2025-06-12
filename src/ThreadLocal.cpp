#include <iostream>
#include <thread>
#include <string>

// Each thread will get its own private copy of 'thread_id' and 'counter'.
thread_local int counter = 0;

void worker_function(const std::string& name) {
    // The first time a thread calls this function, its 'counter' is initialized to 0.
    // On subsequent calls BY THE SAME THREAD, it will retain its previous value.

    for (int i = 0; i < 3; ++i) {
        counter++;
        std::cout << name << ": counter = " << counter
                  << ", address = " << &counter << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

int main() {
    std::cout << "Starting threads..." << std::endl;
    std::thread t1(worker_function, "Thread 1");
    std::thread t2(worker_function, "Thread 2");

    t1.join();
    t2.join();

    // The main thread also has its own copy of the counter.
    std::cout << "Main thread: counter = " << counter
              << ", address = " << &counter << std::endl;

    return 0;
}