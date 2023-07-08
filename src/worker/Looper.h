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
        Looper(std::function<void()> function) : running_(ATOMIC_FLAG_INIT), function_(function) {}

        void Run() {
            while(running_.test()) {
                try {
                    function_();
                } catch (Exception e) {
                    LOG(ERROR) << e.what();
                }
            }
        }

        void Start() {
            if (!running_.test_and_set()) {
                thread_ = std::thread(&Looper::Run, this);
            }
        }

        void Stop() {
            running_.clear();
        }

        bool IsRunning() {
            return running_.test();
        }

        std::string ToString() {
            return "Looper[]";
        }

        virtual ~Looper() {
            LOG(INFO) << ToString() << ".~Looper()";

            thread_.join();
        }

    private:
        std::atomic_flag running_;
        std::function<void()> function_;
        std::thread thread_;
    };

} // worker

#endif //MULTIPATH_PROXY_WORKER_H
