//
// Created by Matthias Hofstätter on 30.05.23.
//

#ifndef MULTIPATH_PROXY_WORKER_H
#define MULTIPATH_PROXY_WORKER_H

#include <atomic>
#include <functional>
#include <thread>
#include <glog/logging.h>

#include "../exception/Exception.h"

namespace worker {

    class Looper {
    public:
        Looper(std::function<void()> function) : stop_(ATOMIC_FLAG_INIT), function_(function), thread_(&Looper::Run, this) {}

        void Run() {
            while(!stop_.test()) {
                try {
                    function_();
                } catch (Exception e) {
                    LOG(ERROR) << e.what();
                }
            }
        }

        void Stop() {
            stop_.test_and_set();
        }

        void Join() {
            thread_.join();
        }

        std::string ToString() {
            return "Looper[]";
        }

        virtual ~Looper() {
            DLOG(INFO) << ToString() << ".~Looper()";

            thread_.join();
        }

    private:
        std::atomic_flag stop_;
        std::function<void()> function_;
        std::thread thread_;
    };

} // worker

#endif //MULTIPATH_PROXY_WORKER_H
