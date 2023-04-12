//
// Created by Matthias Hofstätter on 11.04.23.
//

#ifndef MULTIPATH_PROXY_ICONNECTION_H
#define MULTIPATH_PROXY_ICONNECTION_H

#endif //MULTIPATH_PROXY_ICONNECTION_H

#include <vector>


namespace net {

    class IConnection {
    public:
        virtual std::vector<char> Recv(int flags) = 0;
        virtual void Send(std::vector<char> buf, int flags) = 0;
        virtual void Close() = 0;
    };
}
