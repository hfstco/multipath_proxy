//
// Created by Matthias Hofstätter on 09.09.23.
//

#ifndef MPP_LOOP_H
#define MPP_LOOP_H

#include <liburing.h>
#include <thread>

namespace loop {

    class Loop {
    public:
        Loop(Loop const&) = delete;
        void operator=(Loop const&) = delete;

        static Loop& getInstance()
        {
            static Loop loop; // Guaranteed to be destroyed.
            // Instantiated on first use.
            return loop;
        }

        virtual ~Loop();

    private:
        // loop
        std::atomic<bool> _running;
        std::thread _thread;

        // io_uring
        struct io_uring _ring;

        Loop();

        int run();
    };

} // loop

#endif //MPP_LOOP_H
