//
// Created by Matthias Hofstätter on 01.08.23.
//

#ifndef MPP_STREAM_H
#define MPP_STREAM_H

#include <glog/logging.h>
#include <string>
#include <cstdint>
#include <mutex>
#include <atomic>

#include "picoquic.h"
#include "base/StreamBase.h"

namespace backlog {
    class Chunk;
}

namespace quic {

    class Context;

    class Stream : public StreamBase {
        friend class Context;
    public:
        explicit operator uint64_t() const noexcept
        { return _stream_id; }

        explicit operator uint64_t() const volatile noexcept
        { return _stream_id; }

        Stream(const Stream& stream) = delete;
        Stream& operator=(const Stream& stream) = delete;

        //virtual int prepare_to_send(uint8_t *bytes, size_t length) = 0;
        //virtual int stream_data(uint8_t *bytes, size_t length) = 0;
        //virtual int stream_fin(uint8_t *bytes, size_t length) = 0;

        [[nodiscard]] std::string to_string() const;

        virtual ~Stream();

    protected:
        Stream(Context *context, uint64_t stream_id);

        virtual int callback(picoquic_cnx_t *cnx, uint64_t stream_id, uint8_t *bytes, size_t length,
                             picoquic_call_back_event_t fin_or_event, void *callback_ctx, void *v_stream_ctx);
    };

} // quic

#endif //MPP_STREAM_H
