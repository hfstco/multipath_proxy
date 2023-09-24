//
// Created by Matthias Hofstätter on 09.09.23.
//

#include "Loop.h"

#include <glog/logging.h>

namespace loop {
    struct request {
        int event_type;
        int iovec_count;
        int client_socket;
        struct iovec iov[];
    };

    Loop::Loop() : _ring(), _running(true) {
        io_uring_queue_init(100, &_ring, 0);
    }

    Loop::~Loop() {
        io_uring_queue_exit(&_ring);
    }

    int Loop::run() {
        int ret = 0;

        while (_running && ret == 0) {
            struct io_uring_cqe *cqe;
            int ret = io_uring_wait_cqe(&_ring, &cqe);
            struct request *req = (struct request *) cqe->user_data;
            if (ret < 0)
                LOG(ERROR) << "io_uring_wait_cqe failed.";
                break;
            if (cqe->res < 0) {
                LOG(ERROR) << "async request failed: " << -cqe->res << " for event: " << req->event_type;
                break;
            }

            switch (req->event_type) {

            }
        }

        return ret;
    }
} // loop