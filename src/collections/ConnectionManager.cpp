//
// Created by Matthias Hofstätter on 17.06.23.
//

#include "ConnectionManager.h"
#include "../net/IConnection.h"

namespace collections {
    // add connection to manager
    void ConnectionManager::Insert(net::IConnection *connection) {
        std::lock_guard lock(mutex_);
        std::unordered_map<int, net::IConnection *>::insert({connection->fd(), connection});
    }

    // remove connection from manager
    void ConnectionManager::Erase(net::IConnection *connection) {
        std::lock_guard lock(mutex_);
        std::unordered_map<int, net::IConnection *>::erase(connection->fd());
    }
} // collections