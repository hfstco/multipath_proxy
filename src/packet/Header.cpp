//
// Created by Matthias Hofstätter on 25.04.23.
//

#include "Header.h"

namespace packet {
    Header::Header(TYPE type) : type_(type) {}

    TYPE Header::type() const {
        return type_;
    }

    std::string Header::ToString() {
        switch (type()) {
            case TYPE::FLOW:
                return "H[type=FLOW]";
            case TYPE::HEARTBEAT:
                return "H[type=HEARTBEAT]";
        }
    }

}
