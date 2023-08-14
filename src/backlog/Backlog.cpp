//
// Created by Matthias Hofstätter on 03.08.23.
//

#include "Backlog.h"

#include <cstring>
#include <glog/logging.h>

namespace backlog {

    Backlog::Backlog(bool is_rx) :  _size(0),
                                    _offset(0),
                                    _is_rx(is_rx) {
    }

    Backlog::Backlog() : Backlog(false) {
    }

    uint64_t Backlog::size() {
        return _size.load();
    }

    uint64_t Backlog::offset() {
        return _offset.load();
    }

    uint64_t Backlog::available() {
        return _available.load();
    }

    bool Backlog::Empty() {
        std::lock_guard lock(_mutex);

        return _size.load() == 0;
    }

    void Backlog::Reset() {
        std::lock_guard lock(_mutex);

        for (auto &it : _chunks) {
            delete it.second;
        }

        _chunks.clear();
        _size.store(0);
        _offset.store(0);
    }

    void Backlog::Insert(backlog::Chunk *chunk) {
        std::lock_guard lock(_mutex);

        //LOG(INFO) << "Insert(chunk=" << chunk->ToString() << ")";

        // insert
        _chunks.insert({chunk->offset, chunk});

        // update size & TotalBacklog
        _size.fetch_add(chunk->size);
        if (_is_rx) {
            TotalBacklog.fetch_add(chunk->size);
        }

        // update available
        _available.store(0);
        auto current = _chunks.begin();
        if (current->second->offset <= _offset.load()) { // && _offset.load() < current->second->offset + current->second->size
            // first chunk
            _available.fetch_add(current->second->size);
            uint64_t lastOffset = current->second->offset + current->second->size;
            current++;
            while (current != _chunks.end() && lastOffset == current->second->offset) {
                _available.fetch_add(current->second->size);
                lastOffset = current->second->offset + current->second->size;
                current++;
            }
            // offset
            _available.fetch_sub(_offset.load() - _chunks.begin()->second->offset);
        }
    }

    backlog::Chunk *Backlog::Next(uint64_t max) {
        std::lock_guard lock(_mutex);

        int get = max;
        if (_available.load() < max) {
            get = _available.load();
        }

        Chunk *chunk = nullptr;
        if (get > 0) {
            chunk = new Chunk;
            chunk->data = static_cast<unsigned char *>(malloc(max));
            chunk->offset = _offset.load();
            chunk->size = get;

            uint64_t written = 0;
            auto current = _chunks.begin();
            while (current != _chunks.end() && written != get) {
                uint64_t offset = (_offset.load() - current->second->offset);
                uint64_t toRead = std::min((get - written), current->second->size - offset);
                memcpy(chunk->data + written, current->second->data + offset, toRead);
                _offset.fetch_add(toRead);
                written += toRead;

                // remove chunk if read completely
                if (offset + toRead == current->second->size) {
                    delete current->second;
                    current++;
                    _chunks.erase(std::prev(current));
                }
            }

            _available.fetch_sub(written);
            _size.fetch_sub(written);
            if (_is_rx) {
                TotalBacklog.fetch_sub(written);
            }
            LOG(INFO) << "Next(" << max << ") -> " << chunk->ToString();
        } else {
            //LOG(INFO) << "Next(" << max << ") -> nullptr";
        }

        return chunk;
    }

    Backlog::~Backlog() {
        Reset();
    }
} // backlog
