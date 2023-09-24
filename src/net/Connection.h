//
// Created by Matthias Hofstätter on 04.03.23.
//

#ifndef MULTIPATH_PROXY_CONNECTION_H
#define MULTIPATH_PROXY_CONNECTION_H

#include <sys/ioctl.h>
#include <mutex>
#include <glog/logging.h>
#include <atomic>

#include "TcpSocket.h"
#include "../args/ARGS.h"


namespace net {

    template <IsSocket S, IsSockAddr SA>
    class Listener;

    namespace ipv4 {
        class TcpConnection;
    }

    namespace ipv6 {
        class TcpConnection;
    }

    template <IsSocket S, IsSockAddr SA>
    class Connection : public S {
        friend Listener<S, SA>;

    public:
        int fd() {
            return S::fd();
        }

        ssize_t Recv(unsigned char *data, size_t size, int flags) {
            std::lock_guard lock(recvMutex_);

            ssize_t bytesRead = S::Recv(data, size, flags);

            // metrics
            if (args::METRICS_ENABLED) {
                recvBytes_.fetch_add(bytesRead);
            }

            return bytesRead;
        };

        ssize_t Send(unsigned char *data, size_t size, int flags) {
            std::lock_guard lock(sendMutex_);

            ssize_t bytesWritten = S::Send(data, size, flags | MSG_NOSIGNAL);

            // metrics
            if (args::METRICS_ENABLED) {
                sendBytes_.fetch_add(bytesWritten);
            }

            return bytesWritten;
        };

        short Poll(short events, int timeout = 0) {
            return S::Poll(events, timeout);
        }

        SA GetSockName() {
            return S::GetSockName();
        }

        SA GetPeerName() {
            return S::GetPeerName();
        }

        template<typename T>
        T IoCtl(unsigned long request) {
            return S::template IoCtl<T>(request);
        }

        uint64_t sendBytes() {
            if (!args::METRICS_ENABLED) {
                return 0;
            }

            return sendBytes_.load();
        }

        float sendDataRate() {
            if (!args::METRICS_ENABLED) {
                return 0;
            }

            // get current bytes and current time
            uint64_t sendBytes = sendBytes_.load();
            auto now = std::chrono::system_clock::now();

            // diff to current and last bytes
            uint64_t bytesSinceLastTimeStamp = sendBytes - lastSendBytes_;
            // get timespam between current und last timestamp
            std::chrono::duration<float> duration = now - lastSendTimeStamp_;
            float timeSinceLastTimestamp = duration.count();

            // save current bytes & time
            lastSendTimeStamp_ = now;
            lastSendBytes_ = sendBytes;

            // calc and return bytes / timestamp
            return bytesSinceLastTimeStamp / timeSinceLastTimestamp / 1048576.0;
        }

        uint64_t recvBytes() {
            if (!args::METRICS_ENABLED) {
                return 0;
            }

            return recvBytes_.load();
        }

        float recvDataRate() {
            if (!args::METRICS_ENABLED) {
                return 0;
            }

            uint64_t recvBytes = recvBytes_.load();
            auto now = std::chrono::system_clock::now();
            uint64_t bytesSinceLastTimeStamp = recvBytes - lastRecvBytes_;
            std::chrono::duration<float> duration = now - lastRecvTimeStamp_;
            float timeSinceLastTimestamp = duration.count();

            lastRecvTimeStamp_ = now;
            lastRecvBytes_ = recvBytes;

            float datarate = bytesSinceLastTimeStamp / timeSinceLastTimestamp / 1048576.0;

            //LOG(INFO) << "fd:" << fd() << ", recvDataRate: " << datarate;

            return datarate;
        }

        int sendBufferSize() {
            return IoCtl<int>(TIOCOUTQ);
        }

        int recvBufferSize() {
            return IoCtl<int>(FIONREAD);
        }

        std::string ToString() {
            return "Connection[fd=" + std::to_string(S::fd()) + "]";
        }

        void Close() {
            S::Close();
        }

        void Shutdown(int how) {
            S::Shutdown(how);
        }

        virtual ~Connection() {
            VLOG(2) << ToString() << ".~Connection()";
        }

    protected:
        explicit Connection(SA peeraddr) : Connection() {
            VLOG(2) << "Connection(peeraddr=" << peeraddr.to_string() << ") * " << ToString();

            S::Connect(peeraddr);
        }

        Connection(SA peeraddr, SA sockaddr) : Connection() {
            VLOG(2) << "Connection(peeraddr=" << peeraddr.to_string() << ", sockaddr=" << sockaddr.to_string() << ") * " << ToString();

            S::Bind(peeraddr);
            S::Connect(peeraddr);
        };

        explicit Connection(int fd) : S(fd) {
            VLOG(2) << "Connection(fd=" + std::to_string(fd) + ") * " << ToString();
        }

    private:
        std::mutex recvMutex_;
        std::mutex sendMutex_;

        // metrics
        std::atomic<uint64_t> recvBytes_ = 0;
        std::atomic<uint64_t> sendBytes_ = 0;

        uint64_t lastRecvBytes_ = 0;
        std::chrono::time_point<std::chrono::system_clock> lastRecvTimeStamp_ = std::chrono::system_clock::now();
        uint64_t lastSendBytes_ = 0;
        std::chrono::time_point<std::chrono::system_clock> lastSendTimeStamp_ = std::chrono::system_clock::now();

        Connection() {
            S::SetSockOpt(SOL_SOCKET, SO_REUSEADDR, 1);
        };
    };

}


#endif //MULTIPATH_PROXY_CONNECTION_H
