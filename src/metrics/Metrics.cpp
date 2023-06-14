//
// Created by Matthias Hofstätter on 05.06.23.
//

#include "Metrics.h"

#include <glog/logging.h>
#include <fstream>
#include <sys/ioctl.h>
#include <chrono>

namespace metrics {
    Metrics::Metrics() : file_(new std::ofstream("metrics.csv", std::ios::out)), metricsLooper_(std::bind(&Metrics::WriteMetricsToDisk, this)) {
        DLOG(INFO) << "Metrics() * " << ToString();

        *file_ << "timestamp,fd,recvBytes,recvPackets,recvDataRate,recvBufferFillLevel,sendBytes,sendPackets,sendDataRate,sendBufferFillLevel" << std::endl;
    }

    void Metrics::addConnection(int fd) {
        connectionMetrics_.insert({fd, new ConnectionMetric(fd)});
    }

    ConnectionMetric *Metrics::getConnection(int fd) {
        return connectionMetrics_.find(fd)->second;
    }

    void Metrics::removeConnection(int fd) {
        delete connectionMetrics_.find(fd)->second;
        connectionMetrics_.erase(fd);
    }

    void Metrics::PrintMetrics() {
        LOG(INFO) << "METRICS:\n";
    }

    void Metrics::WriteMetricsToDisk() {
        if (!connectionMetrics_.empty()) {
            for (auto it = connectionMetrics_.cbegin(); it != connectionMetrics_.cend(); ++it) {
                ConnectionMetric *connectionMetric = it->second;
                int recvBufferFillLevel = 0;
                int sendBufferFillLevel = 0;
                ioctl(it->first, FIONREAD, &recvBufferFillLevel);
                ioctl(it->first, TIOCOUTQ, &sendBufferFillLevel);
                int recvDataRate = connectionMetric->recvDataRate();
                int sendDataRate = connectionMetric->sendDataRate();

                // timestamp
                *file_ << std::chrono::duration_cast<std::chrono::milliseconds>(
                        std::chrono::system_clock::now().time_since_epoch()).count() << ",";

                // fd
                *file_ << it->first << ",";

                // data
                *file_ << connectionMetric->recvBytes() << "," << connectionMetric->recvPackets() << ","
                       << connectionMetric->recvDataRate() << "," << recvBufferFillLevel << ","
                       << connectionMetric->sendBytes() << "," << connectionMetric->sendPackets() << ","
                       << connectionMetric->sendDataRate() << "," << sendBufferFillLevel;

                //LOG(INFO) << "METRIC: fd[" << it->first << "] sDataRate: " << sendDataRate << ", sBufferLevel: " << sendBufferFillLevel << ", rDataRate: " << recvDataRate << ", rBufferFillLevel: " << recvBufferFillLevel;

                // endl;
                *file_ << std::endl;
            }
        }

        usleep(10);
    }

    std::string Metrics::ToString() {
        return "Metrics[]";
    }

    Metrics::~Metrics() {
        // close file
        file_->close();
        delete file_;

        DLOG(INFO) << ToString() << ".~Metrics()";
    }
} // metrics