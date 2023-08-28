//
// Created by Matthias Hofstätter on 25.08.23.
//

#include "Path.h"

#include <cassert>

#include "Context.h"
#include "glog/logging.h"

namespace quic {

    Path::Path(Context *context, uint64_t unique_path_id) : PathBase(context, unique_path_id) {
        VLOG(2) << "*" << to_string();
    }

    int Path::callback(picoquic_cnx_t *cnx, uint64_t stream_id, uint8_t *bytes, size_t length,
                       picoquic_call_back_event_t fin_or_event, void *callback_ctx, void *v_stream_ctx) {
        assert(false);
        return 0;
    }

    std::string Path::to_string() const {
        return "Path[context=" + ((Context *)_context)->to_string() + ", unique_path_id=" + std::to_string(_unique_path_id) + "]";
    }

    Path::~Path() {
        VLOG(2) << "~" << to_string();
        //_context->delete_path(this);
    }
} // quic