//
// Created by Matthias Hofstätter on 25.08.23.
//

#ifndef MPP_PATH_H
#define MPP_PATH_H

#include <cstdint>
#include <mutex>
#include <picoquic.h>
#include "base/PathBase.h"

namespace quic {

    class Context;
    class FlowContext;

    class Path : public PathBase {
        friend class Context;
        friend class FlowContext;
    public:
        explicit operator uint64_t() const noexcept
        { return _unique_path_id; }

        explicit operator uint64_t() const volatile noexcept
        { return _unique_path_id; }

        Path(const Path& stream) = delete;
        Path& operator=(const Path& stream) = delete;

        [[nodiscard]] std::string to_string() const;

        virtual ~Path();

    protected:
        Path(Context *context, uint64_t  unique_path_id);

        virtual int callback(picoquic_cnx_t *cnx, uint64_t stream_id, uint8_t *bytes, size_t length,
                             picoquic_call_back_event_t fin_or_event, void *callback_ctx, void *v_stream_ctx);
    };

} // quic

#endif //MPP_PATH_H
