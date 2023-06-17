//
// Created by Matthias Hofstätter on 17.06.23.
//

#include "ConnectionManager.h"
#include "../net/IConnection.h"

namespace collections {
    void ConnectionManager::Insert(net::IConnection *connection) {
        std::unordered_map<int, net::IConnection *>::insert({connection->fd(), connection});
    }

    void ConnectionManager::Erase(net::IConnection *connection) {
        std::unordered_map<int, net::IConnection *>::erase(connection->fd());
    }
} // collections