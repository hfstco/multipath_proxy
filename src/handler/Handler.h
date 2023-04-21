//
// Created by Matthias Hofstätter on 14.04.23.
//

#ifndef MULTIPATH_PROXY_HANDLER_H
#define MULTIPATH_PROXY_HANDLER_H

#include <thread>
#include "glog/logging.h"

namespace handler {

    class Handler {
    public:
        void Stop();

        virtual ~Handler();

    protected:
        Handler(useconds_t sleep = 100000) : sleep_(sleep), thread_(new std::thread(&Handler::Run, this)) {}

        virtual void Loop() = 0;

    private:
        bool stop_ = false;
        useconds_t sleep_;
        std::thread *thread_;

        void Run();
    };

} // handler

#endif //MULTIPATH_PROXY_HANDLER_H
