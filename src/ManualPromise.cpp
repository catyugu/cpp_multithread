#include <iostream>
#include <thread>
#include <future>
#include <string>

// This worker thread takes a promise that it will fulfill.
void worker_thread(std::promise<std::string> promise) {
    try {
        // Do some work...
        std::this_thread::sleep_for(std::chrono::seconds(2));
        // Fulfill the promise with a value.
        promise.set_value("Here is the data!");
    } catch (...) {
        // Or set an exception if something went wrong.
        promise.set_exception(std::current_exception());
    }
}

int main() {
    // 1. Create a promise.
    std::promise<std::string> my_promise;

    // 2. Get the associated future from the promise.
    auto data_future = my_promise.get_future();

    // 3. Move the promise into the worker thread. Promises cannot be copied.
    std::thread t(worker_thread, std::move(my_promise));

    std::cout << "Main thread is waiting for the promise to be fulfilled..." << std::endl;

    // 4. Get the result from the future.
    try {
        std::string data = data_future.get();
        std::cout << "Received data: " << data << std::endl;
    } catch (const std::exception& e) {
        std::cout << "Exception from worker thread: " << e.what() << std::endl;
    }

    t.join();
    return 0;
}