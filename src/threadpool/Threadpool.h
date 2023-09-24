//
// Created by Matthias Hofstätter on 06.09.23.
//

#ifndef MPP_THREADPOOL_H
#define MPP_THREADPOOL_H

#include "../BS/BS_thread_pool_light.h"

// singleton pattern
// https://stackoverflow.com/questions/1008019/c-singleton-design-pattern
class Threadpool : public BS::thread_pool_light {
public:
    static Threadpool& get()
    {
        static Threadpool threadpool = Threadpool(4); // Guaranteed to be destroyed.
        // Instantiated on first use.
        return threadpool;
    }

    // C++ 11
    // =======
    // We can use the better technique of deleting the methods
    // we don't want.
    Threadpool(Threadpool const&) = delete;
    void operator=(Threadpool const&) = delete;

    // Note: Scott Meyers mentions in his Effective Modern
    //       C++ book, that deleted functions should generally
    //       be public as it results in better error messages
    //       due to the compilers behavior to check accessibility
    //       before deleted status
private:
    Threadpool(const unsigned int thread_count) : BS::thread_pool_light(thread_count) {};                    // Constructor? (the {} brackets) are needed here.
};

#define THREADPOOL Threadpool::get()

#endif //MPP_THREADPOOL_H
