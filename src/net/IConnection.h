//
// Created by Matthias Hofstätter on 17.06.23.
//

#ifndef MULTIPATH_PROXY_ICONNECTION_H
#define MULTIPATH_PROXY_ICONNECTION_H

namespace net {

    class IConnection {
    public:
        virtual int fd() = 0;

        virtual ssize_t Recv(unsigned char *data, size_t size, int flags) = 0;
        virtual ssize_t Send(unsigned char *data, size_t size, int flags) = 0;

        virtual int recvBufferSize() = 0;
        virtual int sendBufferSize() = 0;

        // metrics
        virtual uint64_t recvBytes() = 0;
        virtual uint64_t sendBytes() = 0;

        virtual float recvDataRate() = 0;
        virtual float sendDataRate() = 0;
    };
} // net

#endif //MULTIPATH_PROXY_ICONNECTION_H
