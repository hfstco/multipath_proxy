//
// Created by Matthias Hofstätter on 30.05.23.
// https://stackoverflow.com/questions/1008019/c-singleton-design-pattern
//

#ifndef MULTIPATH_PROXY_CONTEXT_H
#define MULTIPATH_PROXY_CONTEXT_H

#include "../collections/FlowMap.h"
#include "../net/Bond.h"
#include "../task/ThreadPool.h"

namespace context {

    class Context {
    public:
        Context(Context const&) = delete;
        void operator=(Context const&) = delete;

        collections::FlowMap *flows() const {
            return flows_;
        }

        /*task::ThreadPool *threadPool() const {
            return threadPool_;
        }*/

        static Context &GetDefaultContext() {
            static Context context;
            return context;
        }

        virtual ~Context() {
            delete flows_;
            //delete threadPool_;
        }

    private:
        collections::FlowMap *flows_;
        //task::ThreadPool *threadPool_;

        Context() : flows_(new collections::FlowMap())/*, threadPool_(new task::ThreadPool())*/ {}
    };

} // context

#endif //MULTIPATH_PROXY_CONTEXT_H
