/*
//
// Created by Matthias Hofstätter on 05.07.23.
//

#ifndef MULTIPATH_PROXY_QUICCONNECTION_H
#define MULTIPATH_PROXY_QUICCONNECTION_H

#include <quiche.h>

#include "SockAddr.h"

#include "../packet/FlowPacket.h"
#include "../worker/Looper.h"


namespace net {
    static void debug_log(const char *line, void *argp) {
        fprintf(stderr, "%s\n", line);
    }

    class QuicConnection {
    public:
        QuicConnection(net::ipv4::SockAddr_In sockAddr, bool listener = false);

        //void Send(packet::FlowPacket *flowPacket, uint64_t streamId);

        quiche_stats ConnectionStats();
        quiche_path_stats PathStats(size_t idx);

    protected:
        void RecvLoop();
        void SendLoop();
    private:
        int sock_;

        quiche_conn *conn_;
        mutable std::mutex connMutex_;
        quiche_config *config_;

        net::ipv4::SockAddr_In sockAddr_;
        socklen_t sockAddrLen_;
        net::ipv4::SockAddr_In peerAddr_;
        socklen_t peerAddrLen_;

        worker::Looper recvLooper_;
        worker::Looper sendLooper_;
    };

} // net

#endif //MULTIPATH_PROXY_QUICCONNECTION_H
*/
