#include <iostream>
#include <future>
#include <chrono>
#include <thread>

int long_computation() {
    std::cout << "Computation started..." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(2));
    // This could also throw an exception
    // throw std::runtime_error("Failed to compute!");
    std::cout << "Computation finished." << std::endl;
    return 114514;
}

int main() {
    std::cout << "Starting long computation in the background." << std::endl;

    // Launch the function asynchronously. The type is std::future<int>
    // because the function returns an int.
    // std::launch::async is a policy that ensures it runs in a new thread.
    auto result_future = std::async(std::launch::async, long_computation);

    std::cout << "Main thread continues doing other work..." << std::endl;
    // ... do other things here ...

    std::cout << "Now waiting for the result..." << std::endl;
    try {
        // .get() will block here until the future is ready.
        int result = result_future.get();
        std::cout << "The result is: " << result << std::endl;
    } catch (const std::exception& e) {
        std::cout << "Exception caught: " << e.what() << std::endl;
    }

    return 0;
}