#include <iostream>
#include <thread>
#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <future>

class ThreadPool {
public:
    ThreadPool(size_t num_threads) : stop(false) {
        // Create the specified number of worker threads.
        for (size_t i = 0; i < num_threads; ++i) {
            workers.emplace_back([this] {
                while (true) {
                    std::function<void()> task;

                    { // Acquire lock to check the task queue.
                        std::unique_lock<std::mutex> lock(this->queue_mutex);

                        // Wait until there's a task or the pool is stopped.
                        this->condition.wait(lock, [this] {
                            return this->stop || !this->tasks.empty();
                        });

                        // If the pool is stopped and the queue is empty, exit the thread.
                        if (this->stop && this->tasks.empty()) {
                            return;
                        }

                        // Get the next task from the queue.
                        task = std::move(this->tasks.front());
                        this->tasks.pop();
                    } // Release lock.

                    // Execute the task.
                    task();
                }
            });
        }
    }

    // Function to submit a new task to the pool.
    // It uses a packaged_task to get a future back.
    template<class F, class... Args>
    auto submit(F&& f, Args&&... args) -> std::future<decltype(f(args...))> {
        using return_type = decltype(f(args...));

        auto task = std::make_shared<std::packaged_task<return_type()>>(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...)
        );

        std::future<return_type> res = task->get_future();
        {
            std::unique_lock<std::mutex> lock(queue_mutex);

            if (stop) {
                throw std::runtime_error("submit on stopped ThreadPool");
            }

            tasks.emplace([task](){ (*task)(); });
        }
        condition.notify_one();
        return res;
    }

    // Destructor joins all threads.
    ~ThreadPool() {
        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            stop = true;
        }
        condition.notify_all(); // Wake up all threads to exit.
        for (std::thread &worker : workers) {
            worker.join();
        }
    }

private:
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;

    std::mutex queue_mutex;
    std::condition_variable condition;
    bool stop;
};

// --- Example Usage ---
int main() {
    // Create a pool with 4 worker threads.
    ThreadPool pool(4);

    // Submit tasks and get futures back.
    auto future1 = pool.submit([] {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        return 1;
    });

    auto future2 = pool.submit([](int x, int y){ return x + y; }, 5, 3);

    std::cout << "Tasks submitted. Main thread continues." << std::endl;

    // Get results from the futures. This will block until the tasks are complete.
    std::cout << "Result 1: " << future1.get() << std::endl;
    std::cout << "Result 2: " << future2.get() << std::endl;

    return 0; // The pool's destructor will be called here, joining all threads.
}