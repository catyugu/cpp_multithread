#include <iostream>
#include <thread>
#include <chrono>

// This function will be executed by the new thread.
void printNumbers() {
    std::cout << "Worker thread starting..." << std::endl;
    for (int i = 1; i <= 5; ++i) {
        std::cout << "Worker: " << i << std::endl;
        // Sleep to simulate work and make context switching more visible.
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    std::cout << "Worker thread finished." << std::endl;
}

int main() {
    // Create a new thread and tell it to execute the printNumbers function.
    std::thread workerThread(printNumbers);

    std::cout << "Main thread starting..." << std::endl;
    for (char c = 'A'; c <= 'E'; ++c) {
        std::cout << "Main:   " << c << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    // IMPORTANT: Wait for the worker thread to finish before the main function exits.
    workerThread.join();

    std::cout << "Main thread finished." << std::endl;

    return 0;
}