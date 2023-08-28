//
// Created by Matthias Hofstätter on 27.08.23.
//

#ifndef MPP_STREAMBASE_H
#define MPP_STREAMBASE_H

#include <cstdint>
#include <picoquic.h>

namespace quic {
    class ContextBase;

    class StreamBase {
        friend class ContextBase;
    public:
        int set_stream_path_affinity(uint64_t unique_path_id);
        int mark_direct_receive_stream(picoquic_stream_direct_receive_fn direct_receive_fn, void* direct_receive_ctx);
        int set_app_stream_ctx(void* app_stream_ctx);
        void unlink_app_stream_ctx();
        int mark_active_stream(int is_active, void* v_stream_ctx);
        int set_stream_priority(uint8_t stream_priority);
        int mark_high_priority_stream(int is_high_priority);
        int add_to_stream(const uint8_t* data, size_t length, int set_fin);
        void reset_stream_ctx();
        int add_to_stream_with_ctx(const uint8_t * data, size_t length, int set_fin, void * app_stream_ctx);
        int reset_stream(uint64_t local_stream_error);
        int open_flow_control(uint64_t expected_data_size);
        int stop_sending(uint16_t local_stream_error);
        int discard_stream(uint16_t local_stream_error);
        uint64_t get_remote_stream_error();

        virtual ~StreamBase();

    protected:
        ContextBase *_context = nullptr;
        uint64_t _stream_id = 0;

        StreamBase(ContextBase *context, uint64_t stream_id);

    private:
        static int stream_data_cb(picoquic_cnx_t* cnx, uint64_t stream_id, uint8_t* bytes, size_t length, picoquic_call_back_event_t fin_or_event, void* callback_ctx, void* v_stream_ctx);
        static int stream_direct_receive(picoquic_cnx_t* cnx, uint64_t stream_id, int fin, const uint8_t* bytes, uint64_t offset, size_t length, void* direct_receive_ctx);
    };

} // quic

#endif //MPP_STREAMBASE_H
