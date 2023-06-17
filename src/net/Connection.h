//
// Created by Matthias Hofstätter on 04.03.23.
//

#ifndef MULTIPATH_PROXY_CONNECTION_H
#define MULTIPATH_PROXY_CONNECTION_H

#include <sys/ioctl.h>
#include <mutex>
#include <glog/logging.h>

#include "TcpSocket.h"
#include "IConnection.h"
#include "../args/Args.h"

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
    class Connection : public S, public IConnection {
        friend Listener<S, SA>;

    public:
        int fd() {
            return S::fd();
        }

        ssize_t Recv(unsigned char *data, size_t size, int flags) override {
            if (args::METRICS_ENABLED) {
                recvBytes_.fetch_add(size);
            }

            return S::Recv(data, size, flags);
        };

        ssize_t Send(unsigned char *data, size_t size, int flags) override {
            if (args::METRICS_ENABLED) {
                sendBytes_.fetch_add(size);
            }

            return S::Send(data, size, flags | MSG_NOSIGNAL);
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

        uint64_t sendBytes() override {
            if (!args::METRICS_ENABLED) {
                return 0;
            }

            return sendBytes_.load();
        }

        float sendDataRate() override {
            if (!args::METRICS_ENABLED) {
                return 0;
            }

            uint64_t sendBytes = sendBytes_.load();
            auto now = std::chrono::system_clock::now();

            uint64_t bytesSinceLastTimeStamp = sendBytes - lastSendBytes_.load();
            float timeSinceLastTimestamp = std::chrono::duration_cast<std::chrono::seconds>(now - lastSendTimeStamp_).count();

            lastSendTimeStamp_ = now;
            lastSendBytes_ = sendBytes;

            return bytesSinceLastTimeStamp / timeSinceLastTimestamp;
        }

        uint64_t recvBytes() override {
            if (!args::METRICS_ENABLED) {
                return 0;
            }

            return recvBytes_.load();
        }

        float recvDataRate() override {
            if (!args::METRICS_ENABLED) {
                return 0;
            }

            uint64_t recvBytes = recvBytes_.load();
            auto now = std::chrono::system_clock::now();

            uint64_t bytesSinceLastTimeStamp = recvBytes - lastRecvBytes_.load();
            float timeSinceLastTimestamp = std::chrono::duration_cast<std::chrono::seconds>(now - lastRecvTimeStamp_).count();

            lastRecvTimeStamp_ = now;
            lastRecvBytes_ = recvBytes;

            return bytesSinceLastTimeStamp / timeSinceLastTimestamp;
        }

        int sendBufferSize() override {
            return IoCtl<int>(TIOCOUTQ);
        }

        int recvBufferSize() override {
            return IoCtl<int>(FIONREAD);
        }

        std::string ToString() {
            /*std::stringstream stringStream;
            stringStream << "Connection[fd=" << std::to_string(S::fd()) << ", sockName=";
            try {
                stringStream << S::GetSockName().ToString();
            } catch (SocketErrorException e) {
                stringStream << e.ToString();
            }
            stringStream << ", peerAddr=";
            try {
                stringStream << S::GetPeerName().ToString();
            } catch (SocketErrorException e) {
                stringStream << e.ToString();
            }
            stringStream << "]";
            return stringStream.str();*/
            return "Connection[fd=" + std::to_string(S::fd()) + "]";
        }

        void Close() {
            S::Close();
        }

        void Shutdown(int how) {
            S::Shutdown(how);
        }

        virtual ~Connection() {
            DLOG(INFO) << ToString() << ".~Connection()";

            //context_->connections()->Erase(this);
        }

    protected:
        Connection(SA peeraddr) : Connection() {
            DLOG(INFO) << "Connection(peeraddr=" << peeraddr.ToString() << ") * " << ToString();

            S::Connect(peeraddr);
        }

        Connection(SA peeraddr, SA sockaddr) : Connection() {
            DLOG(INFO) << "Connection(peeraddr=" << peeraddr.ToString() << ", sockaddr=" << sockaddr.ToString() << ") * " << ToString();

            S::Bind(peeraddr);
            S::Connect(peeraddr);
        };

        Connection(int fd) : S(fd) {
            DLOG(INFO) << "Connection(fd=" + std::to_string(fd) + ") * " << ToString();
        }

    private:
        //context::Context *context_ = &context::Context::GetDefaultContext();

        std::mutex recvMutex_;
        std::mutex sendMutex_;

        //metrics
        std::atomic<uint64_t> recvBytes_;
        std::atomic<uint64_t> sendBytes_;

        std::atomic<uint64_t> lastRecvBytes_;
        std::chrono::time_point<std::chrono::system_clock> lastRecvTimeStamp_ = std::chrono::system_clock::now();
        std::atomic<uint64_t> lastSendBytes_;
        std::chrono::time_point<std::chrono::system_clock> lastSendTimeStamp_ = std::chrono::system_clock::now();

        Connection() {
            S::SetSockOpt(SOL_SOCKET, SO_REUSEADDR, 1);

            //context_->connections()->Insert(this);
        };
    };

}


#endif //MULTIPATH_PROXY_CONNECTION_H
