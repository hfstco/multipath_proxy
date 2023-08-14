//
// Created by Matthias Hofstätter on 06.06.23.
//
// https://man7.org/linux/man-pages/man7/tcp.7.html
//
//  tcp_wmem (since Linux 2.4)
//              This is a vector of 3 integers: [min, default, max].
//              These parameters are used by TCP to regulate send buffer
//              sizes.  TCP dynamically adjusts the size of the send
//              buffer from the default values listed below, in the range
//              of these values, depending on memory available.
//
//      min     Minimum size of the send buffer used by each TCP
//              socket.  The default value is the system page size.
//              (On Linux 2.4, the default value is 4 kB.)  This
//              value is used to ensure that in memory pressure
//              mode_t, allocations below this size will still
//              succeed.  This is not used to bound the size of the
//              send buffer declared using SO_SNDBUF on a socket.
//
//      default The default size of the send buffer for a TCP
//              socket.  This value overwrites the initial default
//              buffer size from the generic global
//              /proc/sys/net/core/wmem_default defined for all
//              protocols.  The default value is 16 kB.  If larger
//              send buffer sizes are desired, this value should be
//              increased (to affect all sockets).  To employ large
//              TCP windows, the
//              /proc/sys/net/ipv4/tcp_window_scaling must be set
//              to a nonzero value (default).
//
//max    The maximum size of the send buffer used by each
//        TCP socket.  This value does not override the value
//        in /proc/sys/net/core/wmem_max.  This is not used
//        to limit the size of the send buffer declared using
//        SO_SNDBUF on a socket.  The default value is
//calculated using the formula
//
//        max(65536, min(4 MB, tcp_mem[1]*PAGE_SIZE/128))
//
//(On Linux 2.4, the default value is 128 kB, lowered
//64 kB depending on low-memory systems.)

//The maximum sizes for socket buffers declared via the SO_SNDBUF
//and SO_RCVBUF mechanisms are limited by the values in the
///proc/sys/net/core/rmem_max and /proc/sys/net/core/wmem_max
//        files.  Note that TCP actually allocates twice the size of the
//buffer requested in the setsockopt(2) call, and so a succeeding
//        getsockopt(2) call will not return the same size of buffer as
//requested in the setsockopt(2) call.  TCP uses the extra space
//for administrative purposes and internal kernel structures, and
//the /proc file values reflect the larger sizes compared to the
//actual TCP windows.  On individual connections, the socket buffer
//        size must be set prior to the listen(2) or connect(2) calls in
//order to have it take effect.  See socket(7) for more
//        information.

// https://man7.org/linux/man-pages/man7/socket.7.html
// Unless otherwise noted, optval is a
//       pointer to an int.

#include <gtest/gtest.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <fcntl.h>

#include "../src/net/TcpSocket.h"

TEST(development_tests, write_buffer_default) {
    try {
        // create listener
        net::ipv4::TcpSocket *listeningSocket = net::ipv4::TcpSocket::make();
        listeningSocket->SetSockOpt(SOL_SOCKET, SO_REUSEADDR, 1);
        listeningSocket->Bind(net::ipv4::SockAddr_In("127.0.0.1", 7001));
        listeningSocket->Listen();

        // create remote connection
        net::ipv4::TcpSocket *writeSocket = net::ipv4::TcpSocket::make();
        writeSocket->GetSockOpt<int>(SOL_SOCKET, SO_SNDBUF); // LOG only
        /*int sockbuf = 21000;
        socklen_t socklen = sizeof(sockbuf);
        setsockopt(writeSocket->fd(), SOL_SOCKET, SO_SNDBUF, &sockbuf, socklen);
        sockbuf = 0;
        getsockopt(writeSocket->fd(), SOL_SOCKET, SO_SNDBUF, &sockbuf, &socklen);*/
        writeSocket->Connect(net::ipv4::SockAddr_In("127.0.0.1", 7001));

        // accept remote connection on local side
        net::ipv4::TcpSocket *readSocket = listeningSocket->Accept();

        int ret = 0, bytes_written;
        size_t queueSize;
        unsigned char buffer[1024];
        do {
            bytes_written = 0;
            bytes_written = writeSocket->Send(buffer, 1024, 0);

            queueSize = 0;
            if((ret = ioctl(writeSocket->fd(), TIOCOUTQ, &queueSize)) == -1) {
                LOG(ERROR) << "ioctl fail.";
            }

            LOG(INFO) << "bytes_written: " << bytes_written << ", queueSize: " << queueSize;
        } while (bytes_written == 1024);

        // close sockets
        delete listeningSocket;
        delete writeSocket;
        delete readSocket;
    } catch (Exception e) {
        LOG(ERROR) << e.what();
        FAIL();
    }
}

TEST(development_tests, write_buffer_set) {
    try {
        // create listener
        net::ipv4::TcpSocket *listeningSocket = net::ipv4::TcpSocket::make();
        listeningSocket->SetSockOpt(SOL_SOCKET, SO_REUSEADDR, 1);
        listeningSocket->Bind(net::ipv4::SockAddr_In("127.0.0.1", 7001));
        listeningSocket->Listen();

        // create remote connection
        net::ipv4::TcpSocket *writeSocket = net::ipv4::TcpSocket::make();

        // set sock queue size
        // https://man7.org/linux/man-pages/man7/socket.7.html
//        The default value is set by the
//        /proc/sys/net/core/rmem_default file, and the maximum
//        allowed value is set by the /proc/sys/net/core/rmem_max !!!
//        file.  The minimum (doubled) value for this option is 256.
        writeSocket->SetSockOpt<int>(SOL_SOCKET, SO_SNDBUF, 10000);
        writeSocket->GetSockOpt<int>(SOL_SOCKET, SO_SNDBUF); // LOG only
        /*int sockbuf = 21000;
        socklen_t socklen = sizeof(sockbuf);
        setsockopt(writeSocket->fd(), SOL_SOCKET, SO_SNDBUF, &sockbuf, socklen);
        sockbuf = 0;
        getsockopt(writeSocket->fd(), SOL_SOCKET, SO_SNDBUF, &sockbuf, &socklen);*/
        writeSocket->Connect(net::ipv4::SockAddr_In("127.0.0.1", 7001));

        // accept remote connection on local side
        net::ipv4::TcpSocket *readSocket = listeningSocket->Accept();

        int ret = 0, bytes_written;
        size_t queueSize;
        unsigned char buffer[1024];
        do {
            bytes_written = 0;
            bytes_written = writeSocket->Send(buffer, 1024, 0);

            queueSize = 0;
            if((ret = ioctl(writeSocket->fd(), TIOCOUTQ, &queueSize)) == -1) {
                LOG(ERROR) << "ioctl fail.";
            }

            LOG(INFO) << "bytes_written: " << bytes_written << ", queueSize: " << queueSize;
        } while (bytes_written == 1024);

        // close sockets
        delete listeningSocket;
        delete writeSocket;
        delete readSocket;
    } catch (Exception e) {
        LOG(ERROR) << e.what();
        FAIL();
    }
}

TEST(development_tests, get_ioctl) {
    try {
        // create listener
        net::ipv4::TcpSocket *listeningSocket = net::ipv4::TcpSocket::make();
        listeningSocket->SetSockOpt(SOL_SOCKET, SO_REUSEADDR, 1);
        listeningSocket->Bind(net::ipv4::SockAddr_In("127.0.0.1", 7001));
        listeningSocket->Listen();

        // create remote connection
        net::ipv4::TcpSocket *writeSocket = net::ipv4::TcpSocket::make();

        // set sock queue size
        // https://man7.org/linux/man-pages/man7/socket.7.html
//        The default value is set by the
//        /proc/sys/net/core/rmem_default file, and the maximum
//        allowed value is set by the /proc/sys/net/core/rmem_max !!!
//        file.  The minimum (doubled) value for this option is 256.
        writeSocket->SetSockOpt<int>(SOL_SOCKET, SO_SNDBUF, 10000);
        writeSocket->GetSockOpt<int>(SOL_SOCKET, SO_SNDBUF); // LOG only
        /*int sockbuf = 21000;
        socklen_t socklen = sizeof(sockbuf);
        setsockopt(writeSocket->fd(), SOL_SOCKET, SO_SNDBUF, &sockbuf, socklen);
        sockbuf = 0;
        getsockopt(writeSocket->fd(), SOL_SOCKET, SO_SNDBUF, &sockbuf, &socklen);*/
        writeSocket->Connect(net::ipv4::SockAddr_In("127.0.0.1", 7001));

        // accept remote connection on local side
        net::ipv4::TcpSocket *readSocket = listeningSocket->Accept();

        int ret = 0, bytes_written;
        size_t queueSize;
        unsigned char buffer[1024];
        do {
            bytes_written = 0;
            bytes_written = writeSocket->Send(buffer, 1024, 0);

            queueSize = writeSocket->IoCtl<int>(TIOCOUTQ);

            LOG(INFO) << "bytes_written: " << bytes_written << ", queueSize: " << queueSize;
        } while (bytes_written == 1024);

        // close sockets
        delete listeningSocket;
        delete writeSocket;
        delete readSocket;
    } catch (Exception e) {
        LOG(ERROR) << e.what();
        FAIL();
    }
}
