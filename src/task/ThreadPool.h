//
// Created by Matthias Hofstätter on 23.05.23.
//

#ifndef MULTIPATH_PROXY_THREADPOOL_H
#define MULTIPATH_PROXY_THREADPOOL_H

#include <functional>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <glog/logging.h>

namespace task {

    class ThreadPool {
    public:
        ThreadPool(size_t numberOfThreads) : stop_(false)  {
            LOG(INFO) << "ThreadPool(" << numberOfThreads << ")";

            for (size_t i = 0; i < numberOfThreads; ++i) {
                std::thread thread([this]() {
                    while (!stop_) {
                        std::function<void()> task;
                        {
                            std::unique_lock lock(mutex_);
                            conditionVariable_.wait(lock, [this] { return stop_ || !tasks_.empty(); });
                            if (stop_ && tasks_.empty()) {
                                return;
                            }
                            task = std::move(tasks_.front());
                            tasks_.pop();
                        }
                        task();
                    }
                });
                threads_.emplace_back(std::move(thread));
            }
        }

        ThreadPool() : ThreadPool(std::thread::hardware_concurrency()) {}

        template<typename F, typename... A>
        void PushTask(F&& function, A&&... args) {
            {
                std::scoped_lock lock(mutex_);
                tasks_.push(std::bind(std::forward<F>(function), std::forward<A>(args)...));
            }

            conditionVariable_.notify_one();

            //LOG(INFO) << "PushTask(" << function.target_type().name() << ") SIZE:" << tasks_.size();
        }

        virtual ~ThreadPool() {
            stop_.store(true);
            conditionVariable_.notify_all();
            for (std::thread &thread : threads_) {
                thread.join();
            }
        }

    private:
        std::vector<std::thread> threads_;
        std::queue<std::function<void()>> tasks_;

        std::mutex mutex_;
        std::condition_variable conditionVariable_;
        std::atomic<bool> stop_;
    };

} // task

#endif //MULTIPATH_PROXY_THREADPOOL_H
