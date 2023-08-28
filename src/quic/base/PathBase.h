//
// Created by Matthias Hofstätter on 27.08.23.
//

#ifndef MPP_PATHBASE_H
#define MPP_PATHBASE_H

#include <cstdint>
#include <picoquic.h>

namespace quic {
    class ContextBase;

    class PathBase {
        friend class ContextBase;
    public:
        int set_app_path_ctx(void * app_path_ctx);
        int abandon_path(uint64_t reason, char const* phrase);
        int refresh_path_connection_id();
        int get_path_quality(picoquic_path_quality_t * quality);
        int subscribe_to_quality_update_per_path(uint64_t pacing_rate_delta, uint64_t rtt_delta);
        int mark_datagram_ready_path(int is_path_ready);

        virtual ~PathBase();

    protected:
        ContextBase *_context = nullptr;
        uint64_t _unique_path_id = 0;
        picoquic_path_quality_t _quality;

        PathBase(ContextBase *context, uint64_t unique_path_id);

    private:
        static int stream_data_cb(picoquic_cnx_t* cnx, uint64_t stream_id, uint8_t* bytes, size_t length, picoquic_call_back_event_t fin_or_event, void* callback_ctx, void* v_stream_ctx);
    };

} // quic

#endif //MPP_PATHBASE_H
