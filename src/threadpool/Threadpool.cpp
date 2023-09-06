//
// Created by Matthias Hofstätter on 03.09.23.
//

#include "Threadpool.h"

namespace threadpool {

    Threadpool::Threadpool(size_t nr_threads) {
        for (size_t i = 0; i < nr_threads; ++i)
        {
            std::thread worker([this]() {
                while (true)
                {
                    std::function<void()> task;
                    /* pop a task from queue, and execute it. */
                    {
                        std::unique_lock lock(mtx);
                        cv.wait(lock, [this]() { return stop || !tasks.empty(); });
                        if (stop && tasks.empty())
                            return;
                        /* even if stop = 1, once tasks is not empty, then
                         * excucte the task until tasks queue become empty
                         */
                        task = std::move(tasks.front());
                        tasks.pop();
                    }
                    task();
                }
            });
            workers.emplace_back(std::move(worker));
        }
    }

    Threadpool::~Threadpool() {
        /* stop thread pool, and notify all threads to finish the remained tasks. */
        {
            std::unique_lock<std::mutex> lock(mtx);
            stop = true;
        }
        cv.notify_all();
        for (auto &worker : workers)
            worker.join();
    }

} // threadpool