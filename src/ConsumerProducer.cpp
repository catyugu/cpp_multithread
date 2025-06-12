#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <string>

std::queue<std::string> data_queue;
std::mutex mtx;
std::condition_variable cv;
bool finished = false; // A flag to signal the end

void producer() {
    for (int i = 0; i < 5; ++i) {
        // Simulate fetching data
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::string data = "Data packet " + std::to_string(i);

        { // Start a new scope for the lock guard
            std::lock_guard<std::mutex> guard(mtx);
            data_queue.push(data);
            std::cout << "Producer: Pushed '" << data << "' to the queue." << std::endl;
        } // Lock is released here

        // Wake up one waiting consumer thread
        cv.notify_one();
    }

    // After finishing, signal the end and notify all consumers
    {
        std::lock_guard<std::mutex> guard(mtx);
        finished = true;
    }
    cv.notify_all(); // Wake up all consumers so they can exit
}

void consumer() {
    while (true) {
        // Create a unique_lock to prepare for waiting
        std::unique_lock<std::mutex> lock(mtx);

        // Wait until the predicate is true.
        // The predicate is: "the queue is not empty OR production is finished".
        cv.wait(lock, [] { return !data_queue.empty() || finished; });

        // When wait() returns, we own the lock and know the condition is true.
        if (!data_queue.empty()) {
            // Move data out of the queue while the lock is held
            std::string data = data_queue.front();
            data_queue.pop();
            lock.unlock(); // Unlock early, before processing the data

            std::cout << "Consumer: Processed '" << data << "'" << std::endl;
        } else if (finished) {
            // The queue is empty but the producer is done, so we can exit.
            break; // The lock is released when `lock` goes out of scope
        }
    }
    std::cout << "Consumer: Finished." << std::endl;
}

int main() {
    std::thread prod(producer);
    std::thread cons(consumer);
    std::thread cons2(consumer);

    prod.join();
    cons.join();
    cons2.join();

    return 0;
}