//
// Created by Matthias Hofstätter on 05.06.23.
//

#ifndef MULTIPATH_PROXY_METRICS_H
#define MULTIPATH_PROXY_METRICS_H

#include <stdlib.h>
#include <atomic>

namespace metrics {

    class Metrics {
    public:
        Metrics();

        std::atomic<size_t> avgSatSendQueueSize;
        std::atomic<size_t> maxSatSendQueueSize;

        void PrintMetrics();

        std::string ToString();

        virtual ~Metrics();
    };

} // metrics

#endif //MULTIPATH_PROXY_METRICS_H
