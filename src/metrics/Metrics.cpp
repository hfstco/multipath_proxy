//
// Created by Matthias Hofstätter on 05.06.23.
//

#include "Metrics.h"

#include <glog/logging.h>

namespace metrics {
    Metrics::Metrics() : avgSatSendQueueSize(0), maxSatSendQueueSize(0) {
        DLOG(INFO) << "Metrics() * " << ToString();
    }

    void Metrics::PrintMetrics() {
        LOG(INFO) << "METRICS:\n" <<
                        "avgSatSendQueueSize: " << avgSatSendQueueSize.load() << "\n" <<
                        "maxSatSendQueueSize: " << maxSatSendQueueSize.load();
    }

    std::string Metrics::ToString() {
        return "Metrics[]";
    }

    Metrics::~Metrics() {
        DLOG(INFO) << ToString() << ".~Metrics()";
    }
} // metrics