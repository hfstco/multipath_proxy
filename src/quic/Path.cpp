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
        LOG(INFO) << "New Path width ID=" << _unique_path_id;
    }

    std::string Path::to_string() const {
        return "Path[context=" + ((Context *)_context)->to_string() + ", unique_path_id=" + std::to_string(_unique_path_id) + "]";
    }

    Path::~Path() {
        VLOG(2) << "~" << to_string();
        LOG(INFO) << "Delete Path with ID=" << _unique_path_id;
        //_context->delete_path(this);
    }
} // quic