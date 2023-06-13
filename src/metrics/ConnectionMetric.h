//
// Created by Matthias Hofstätter on 09.06.23.
//

#ifndef MULTIPATH_PROXY_CONNECTIONMETRIC_H
#define MULTIPATH_PROXY_CONNECTIONMETRIC_H

#include <queue>
#include <chrono>
#include <string>
#include <mutex>

namespace metrics {

    struct ConnectionMetricsHistoryEntry {
        ConnectionMetricsHistoryEntry(const std::chrono::time_point<std::chrono::steady_clock> &timeStamp,

                                      uint32_t byteSize) : timeStamp(timeStamp), byteSize(byteSize) {}

        std::chrono::time_point<std::chrono::steady_clock> timeStamp;
        uint32_t byteSize;
    };

    class ConnectionMetric {
    public:
        ConnectionMetric(int fd);

        int recvDataRate();
        uint64_t recvPackets();
        uint64_t recvBytes();

        void AddRecvBytes(uint32_t byteSize);

        int sendDataRate();
        uint64_t sendPackets();
        uint64_t sendBytes();

        void AddSendBytes(uint32_t byteSize);

        std::string ToString();

        virtual ~ConnectionMetric();

    private:
        mutable std::mutex recvMutex_;
        mutable std::mutex sendMutex_;

        const int fd_;

        std::deque<ConnectionMetricsHistoryEntry> recvHistory_;
        uint64_t recvPackets_ = 0;
        uint64_t recvBytes_ = 0;
        std::deque<ConnectionMetricsHistoryEntry> sendHistory_;
        uint64_t sendPackets_ = 0;
        uint64_t sendBytes_ = 0;

        int CalculateDataRate(std::deque<ConnectionMetricsHistoryEntry> deque);
    };

} // metrics

#endif //MULTIPATH_PROXY_CONNECTIONMETRIC_H
