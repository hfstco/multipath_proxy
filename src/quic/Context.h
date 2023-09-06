//
// Created by Matthias Hofstätter on 25.08.23.
//

#ifndef MPP_CONTEXT_H
#define MPP_CONTEXT_H

#include <mutex>
#include <unordered_map>

#include "picoquic.h"
#include "base/ContextBase.h"

#define QUIC_ALPN_NONE "none"

namespace quic {

    class Quic;
    class Path;
    class Stream;
    class FlowContext;

    class Context : public ContextBase {
        friend class ContextBase;
        friend class Quic;
        friend class FlowContext;
    public:
        /*explicit operator picoquic_cnx_t *() const noexcept
        { return _cnx; }

        explicit operator picoquic_cnx_t *() volatile noexcept
        { return _cnx; }*/

        template<class STREAM = Stream, class CONTEXT = Context, class... ARGS>
        STREAM *new_stream(int is_unidir, ARGS... args) {
            static_assert(std::is_base_of<Stream, STREAM>::value); // TODO
            std::lock_guard lock(_mutex);

            uint64_t stream_id = picoquic_get_next_local_stream_id(_cnx, is_unidir) + 4; // TODO + 2

            auto *stream = (STREAM *)new STREAM((CONTEXT *)this, stream_id, args...);

            _streams.insert(std::pair(stream_id, stream));
            picoquic_set_app_stream_ctx(_cnx, stream_id, stream);

            // mark active if local stream id
            /*if (PICOQUIC_IS_CLIENT_STREAM_ID(stream_id) && picoquic_is_client(_cnx) || !PICOQUIC_IS_CLIENT_STREAM_ID(stream_id) && !picoquic_is_client(_cnx) ||
                PICOQUIC_IS_BIDIR_STREAM_ID(stream_id)) {
                mark_active_stream(stream_id, 1, stream);
            }*/

            return stream;
        }

        std::string to_string();

        virtual ~Context();

    protected:
        std::unordered_map<uint64_t, Stream *> _streams;
        std::unordered_map<uint64_t, Path *> _paths;

        // for Quic
        Context(Quic *quic, picoquic_cnx_t *quic_cnx);

        template<class STREAM = Stream, class CONTEXT = Context, class... ARGS>
        STREAM *new_stream(uint64_t stream_id, ARGS... args) {
            static_assert(std::is_base_of<Stream, STREAM>::value); // TODO
            std::lock_guard lock(_mutex);

            auto *stream = new STREAM((CONTEXT *)this, stream_id, args...);
            picoquic_set_app_stream_ctx(_cnx, stream_id, stream);

            // mark active if local stream id
            /*if (PICOQUIC_IS_CLIENT_STREAM_ID(stream_id) && picoquic_is_client(_cnx) || !PICOQUIC_IS_CLIENT_STREAM_ID(stream_id) && !picoquic_is_client(_cnx) ||
                PICOQUIC_IS_BIDIR_STREAM_ID(stream_id)) {
                mark_active_stream(stream_id, 1, stream);
            }*/

            _streams.insert(std::pair(stream_id, stream));

            return stream;
        }

    private:
        Path *new_path(uint64_t unique_path_id);
        void delete_path(uint64_t unique_path_id);

        void delete_stream(uint64_t stream_id);

        static int stream_data_cb(picoquic_cnx_t* cnx, uint64_t stream_id, uint8_t* bytes, size_t length, picoquic_call_back_event_t fin_or_event, void* callback_ctx, void* v_stream_ctx);
        static int stream_direct_receive(picoquic_cnx_t* cnx, uint64_t stream_id, int fin, const uint8_t* bytes, uint64_t offset, size_t length, void* direct_receive_ctx);
    };

} // quic

#endif //MPP_CONTEXT_H
