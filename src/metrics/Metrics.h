//
// Created by Matthias Hofstätter on 05.06.23.
//

#ifndef MULTIPATH_PROXY_METRICS_H
#define MULTIPATH_PROXY_METRICS_H

#include <stdlib.h>
#include <atomic>
#include <map>

#include "../worker/Looper.h"
#include "../context/Context.h"

namespace metrics {

    class Metrics {
    public:
        Metrics();

        /*void addConnection(int fd);
        void removeConnection(int fd);

        ConnectionMetric *getConnection(int fd);*/

        void PrintMetrics();

        std::string ToString();

        virtual ~Metrics();
    private:
        //std::map<int, ConnectionMetric *> connectionMetrics_;

        std::ofstream *file_;

        worker::Looper metricsLooper_;

        void WriteMetricsToDisk();
    };

} // metrics

#endif //MULTIPATH_PROXY_METRICS_H
