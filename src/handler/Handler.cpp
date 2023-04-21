//
// Created by Matthias Hofstätter on 14.04.23.
//

#include "Handler.h"

namespace handler {

    void Handler::Run() {
        while(!this->stop_) {
            usleep(this->sleep_);

            this->Loop();
        }
    }

    void Handler::Stop() {
        this->stop_ = true;
        this->thread_->join();
        delete this->thread_;
    }

    Handler::~Handler() {
        this->Stop();
    }

} // handler