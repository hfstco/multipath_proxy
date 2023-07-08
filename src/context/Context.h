//
// Created by Matthias Hofstätter on 30.05.23.
// https://stackoverflow.com/questions/1008019/c-singleton-design-pattern
//

#ifndef MULTIPATH_PROXY_CONTEXT_H
#define MULTIPATH_PROXY_CONTEXT_H

namespace collections {
    class FlowMap;
    class ConnectionManager;
}

/*namespace task {
    class ThreadPool;
}*/

namespace metrics {
    class Metrics;
}

namespace context {

    class Context {
    public:
        Context(Context const&) = delete;
        void operator=(Context const&) = delete;

        collections::FlowMap *flows() const;

        metrics::Metrics *metrics() const;

        collections::ConnectionManager *connections() const;

        /*task::ThreadPool *threadPool() const;*/

        static Context &GetDefaultContext();

        virtual ~Context();

    private:
        collections::FlowMap *flows_;
        //task::ThreadPool *threadPool_;
        metrics::Metrics *metrics_;
        collections::ConnectionManager *connections_;

        Context();
    };

} // context

#endif //MULTIPATH_PROXY_CONTEXT_H
