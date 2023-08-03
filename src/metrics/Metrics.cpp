//
// Created by Matthias Hofstätter on 05.06.23.
//

#include "Metrics.h"

#include <glog/logging.h>
#include <fstream>
#include <sys/ioctl.h>
#include <chrono>

#include "../collections/ConnectionManager.h"
#include "../net/Connection.h"

namespace metrics {
    Metrics::Metrics() : file_(new std::ofstream("metrics.csv", std::ios::out)), metricsLooper_(std::bind(&Metrics::WriteMetricsToDisk, this)) {
        DLOG(INFO) << "Metrics() * " << ToString();

        metricsLooper_.Start();

        *file_ << "timestamp,fd,recvBytes,recvDataRate,recvBufferFillLevel,sendBytes,sendDataRate,sendBufferFillLevel" << std::endl;
    }

    void Metrics::PrintMetrics() {
        LOG(INFO) << "METRICS:\n";
    }

    void Metrics::WriteMetricsToDisk() {
        if (!context::Context::GetDefaultContext().connections()->empty()) {
            for (auto it = context::Context::GetDefaultContext().connections()->begin(); it != context::Context::GetDefaultContext().connections()->end(); ++it) {
                net::IConnection *connection = it->second;

                // datarate
                float recvDataRate = connection->recvDataRate();
                float sendDataRate = connection->sendDataRate();

                // timestamp
                *file_ << std::chrono::duration_cast<std::chrono::milliseconds>(
                        std::chrono::system_clock::now().time_since_epoch()).count() << ",";

                // fd
                *file_ << it->first << ",";

                // data
                *file_ << connection->recvBytes() << ","
                       << recvDataRate << "," << connection->recvBufferSize() << ","
                       << connection->sendBytes() << ","
                       << sendDataRate << "," << connection->sendBufferSize();

                //LOG(INFO) << connection->fd() << ": recv: " << recvDataRate << ", send: " << sendDataRate;

                *file_ << std::endl;
            }
        }

        usleep(1000000);
    }

    std::string Metrics::ToString() {
        return "Metrics[]";
    }

    Metrics::~Metrics() {
        // close file
        file_->close();
        delete file_;

        metricsLooper_.Stop();

        DLOG(INFO) << ToString() << ".~Metrics()";
    }
} // metrics