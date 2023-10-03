//
// Created by Matthias Hofstätter on 30.06.23.
//

#include "Context.h"

#include "../collections/FlowMap.h"
#include "../metrics/Metrics.h"
#include "../collections/ConnectionManager.h"
#include "../args/Args.h"

namespace context {
    Context::Context() : flows_(new collections::FlowMap()), connections_(new collections::ConnectionManager())/*, threadPool_(new task::ThreadPool())*/ {
        if(args::METRICS_ENABLED) {
            metrics_ = new metrics::Metrics();
        }
    }

    collections::FlowMap *Context::flows() const {
        return flows_;
    }

    metrics::Metrics *Context::metrics() const {
        return metrics_;
    }

    collections::ConnectionManager *Context::connections() const {
        return connections_;
    }

    /*task::ThreadPool *threadPool() const {
        return threadPool_;
    }*/

    Context &Context::GetDefaultContext() {
        static Context context;
        return context;
    }

    Context::~Context() {
        DLOG(INFO) << "~Context()";

        delete flows_;
        //delete threadPool_;
        if(args::METRICS_ENABLED) {
            delete metrics_;
        }
        delete connections_;
    }
}