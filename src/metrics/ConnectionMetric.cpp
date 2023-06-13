//
// Created by Matthias Hofstätter on 09.06.23.
//

#include <glog/logging.h>
#include "ConnectionMetric.h"

namespace metrics {

    ConnectionMetric::ConnectionMetric(int fd) : fd_(fd) {
        LOG(INFO) << "ConnectionMetrics(" << fd << ")";
    }

    int ConnectionMetric::recvDataRate() {
        std::lock_guard lock(recvMutex_);

        return CalculateDataRate(recvHistory_);
    }

    uint64_t ConnectionMetric::recvPackets() {
        std::lock_guard lock(recvMutex_);

        return recvPackets_;
    }

    uint64_t ConnectionMetric::recvBytes() {
        std::lock_guard lock(recvMutex_);

        return recvBytes_;
    }

    void ConnectionMetric::AddRecvBytes(uint32_t byteSize) {
        std::lock_guard lock(recvMutex_);

        recvPackets_ += 1;
        recvBytes_ += byteSize;
        recvHistory_.emplace_front(std::chrono::steady_clock::now(), byteSize);
    }

    int ConnectionMetric::sendDataRate() {
        std::lock_guard lock(sendMutex_);

        return CalculateDataRate(sendHistory_);
    }

    uint64_t ConnectionMetric::sendPackets() {
        std::lock_guard lock(sendMutex_);

        return sendPackets_;
    }

    uint64_t ConnectionMetric::sendBytes() {
        std::lock_guard lock(sendMutex_);

        return sendBytes_;
    }

    void ConnectionMetric::AddSendBytes(uint32_t byteSize) {
        std::lock_guard lock(sendMutex_);

        sendPackets_ += 1;
        sendBytes_ += byteSize;
        sendHistory_.emplace_front(std::chrono::steady_clock::now(), byteSize);
    }

    int ConnectionMetric::CalculateDataRate(std::deque<ConnectionMetricsHistoryEntry> deque) {
        int dataRate = 0;

        std::chrono::time_point<std::chrono::steady_clock> now = std::chrono::steady_clock::now();
        for (auto it = deque.cbegin(); it != deque.cend(); ++it) {
            if (it->timeStamp < now - std::chrono::seconds(1)) {
                deque.erase(it, deque.cend());
                break;
            }

            dataRate += it->byteSize;
        }

        return dataRate;
    }

    std::string ConnectionMetric::ToString() {
        return "ConnectionMetrics[recvDataRate=" + std::to_string(recvDataRate()) + ", recvPackets=" + std::to_string(
                recvPackets()) + ", recvBytes=" + std::to_string(recvBytes()) + "]";
    }

    ConnectionMetric::~ConnectionMetric() {
        LOG(INFO) << "~ConnectionMetrics()";
    }

} // metrics