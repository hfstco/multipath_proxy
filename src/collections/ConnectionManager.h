//
// Created by Matthias Hofstätter on 17.06.23.
//

#ifndef MULTIPATH_PROXY_CONNECTIONMANAGER_H
#define MULTIPATH_PROXY_CONNECTIONMANAGER_H

#include <unordered_map>
#include <mutex>

namespace net {
    class IConnection;
}

namespace collections {

    class ConnectionManager : public std::unordered_map<int, net::IConnection *> {
    public:
        void Insert(net::IConnection *connection);
        void Erase(net::IConnection *connection);

    private:
        std::mutex mutex_;
    };

} // collections

#endif //MULTIPATH_PROXY_CONNECTIONMANAGER_H
