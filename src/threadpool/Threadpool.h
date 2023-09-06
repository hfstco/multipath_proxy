//
// Created by Matthias Hofstätter on 03.09.23.
//

#ifndef MPP_THREADPOOL_H
#define MPP_THREADPOOL_H

#include <functional>
#include <future>
#include <iostream>
#include <queue>
#include <thread>
#include <vector>

// https://www.cnblogs.com/sinkinben/p/16064857.html
namespace threadpool {

    class Threadpool {
    public:
        Threadpool(const Threadpool &) = delete;
        Threadpool(Threadpool &&) = delete;
        Threadpool &operator=(const Threadpool &) = delete;
        Threadpool &operator=(Threadpool &&) = delete;

        Threadpool(size_t nr_threads);
        virtual ~Threadpool();

        template <class F, class... Args>
        std::future<std::result_of_t<F(Args...)>> üenqueue(F &&f, Args &&...args)
        {
            /* The return type of task `F` */
            using return_type = std::result_of_t<F(Args...)>;

            /* wrapper for no arguments */
            auto task = std::make_shared<std::packaged_task<return_type()>>(
                    std::bind(std::forward<F>(f), std::forward<Args>(args)...));

            std::future<return_type> res = task->get_future();
            {
                std::unique_lock lock(mtx);

                if (stop)
                    throw std::runtime_error("The thread pool has been stop.");

                /* wrapper for no returned value */
                tasks.emplace([task]() -> void { (*task)(); });
            }
            cv.notify_one();
            return res;
        }

    private:
        std::vector<std::thread> workers;
        std::queue<std::function<void()>> tasks;

        /* For sync usage, protect the `tasks` queue and `stop` flag. */
        std::mutex mtx;
        std::condition_variable cv;
        bool stop;
    };

} // threadpool

#endif //MPP_THREADPOOL_H
