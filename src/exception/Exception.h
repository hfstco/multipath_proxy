//
// Created by Matthias Hofstätter on 23.02.23.
//

#ifndef MULTIPATH_PROXY_EXCEPTION_H
#define MULTIPATH_PROXY_EXCEPTION_H

#include <exception>
#include <string>


class Exception : std::exception {
public:
    Exception(const char *what_arg) : message_(what_arg) {};

    Exception(std::string what_arg) : message_(what_arg) {};

    const char *what() { return message_.c_str(); }

    const std::string &message() const {
        return message_;
    }

    std::string ToString() {
        return "Exception[" + message_ + "]";
    };

private:
    const std::string message_;
};

// dev exceptions

class NotImplementedError : public Exception {
public:
    NotImplementedError(const char *what_arg) : Exception(what_arg) {};

    NotImplementedError(std::string what_arg) : Exception(what_arg) {};

    std::string ToString() {
        return "NotImplementedError[" + message() + "]";
    };
};

// net exceptions

class NetworkException : public Exception {
public:
    NetworkException(const char *what_arg) : Exception(what_arg) {};

    NetworkException(std::string what_arg) : Exception(what_arg) {};

    std::string ToString() {
        return "NetworkException[" + message() + "]";
    };
};

class SocketErrorException : public Exception {
public:
    SocketErrorException(const char *what_arg) : Exception(what_arg) {};

    SocketErrorException(std::string what_arg) : Exception(what_arg) {};

    std::string ToString() {
        return "SocketErrorException[" + message() + "]";
    };
};

class SocketClosedException: public Exception {
public:
    SocketClosedException(const char *what_arg) : Exception(what_arg) {};

    SocketClosedException(std::string what_arg) : Exception(what_arg) {};

    std::string ToString() {
        return "SocketClosedException[" + message() + "]";
    };
};

class SockAddrErrorException : public Exception {
public:
    SockAddrErrorException(const char *what_arg) : Exception(what_arg) {};

    SockAddrErrorException(std::string what_arg) : Exception(what_arg) {};

    std::string ToString() {
        return "SockAddrErrorException[" + message() + "]";
    };
};

// collections exception

class NotFoundException : public Exception {
public:
    NotFoundException(const char *what_arg) : Exception(what_arg) {};

    NotFoundException(std::string what_arg) : Exception(what_arg) {};

    std::string ToString() {
        return "NotFoundException[" + message() + "]";
    };
};

class IdAlreadyExistsException : public Exception {
public:
    IdAlreadyExistsException(const char *what_arg) : Exception(what_arg) {};

    IdAlreadyExistsException(std::string what_arg) : Exception(what_arg) {};

    std::string ToString() {
        return "IdAlreadyExistsException[" + message() + "]";
    };
};

// utils exceptions

class UtilsException : public Exception {
public:
    UtilsException(const char *what_arg) : Exception(what_arg) {};

    UtilsException(std::string what_arg) : Exception(what_arg) {};

    std::string ToString() {
        return "UtilsException[" + message() + "]";
    };
};

// flow exception

class ConnectionClosedException : public Exception {
public:
    ConnectionClosedException(const char *what_arg) : Exception(what_arg) {};

    ConnectionClosedException(std::string what_arg) : Exception(what_arg) {};

    std::string ToString() {
        return "ConnectionClosedException[" + message() + "]";
    };
};

class GotClosePacketException : public Exception {
public:
    GotClosePacketException(const char *what_arg) : Exception(what_arg) {};

    GotClosePacketException(std::string what_arg) : Exception(what_arg) {};

    std::string ToString() {
        return "GotClosePacketException[" + message() + "]";
    };
};

// listener exception

class ListenerClosedException : public Exception {
public:
    ListenerClosedException(const char *what_arg) : Exception(what_arg) {};

    ListenerClosedException(std::string what_arg) : Exception(what_arg) {};

    std::string ToString() {
        return "ListenerClosedException[" + message() + "]";
    };
};

// bond exception

class SocketAlreadyConnectedException : public Exception {
public:
    SocketAlreadyConnectedException(const char *what_arg) : Exception(what_arg) {};

    SocketAlreadyConnectedException(std::string what_arg) : Exception(what_arg) {};

    std::string ToString() {
        return "SocketAlreadyConnectedException[" + message() + "]";
    };
};

// buffer exception

class BufferReallocError : public Exception {
public:
    BufferReallocError(const char *what_arg) : Exception(what_arg) {};

    BufferReallocError(std::string what_arg) : Exception(what_arg) {};

    std::string ToString() {
        return "BufferReallocError[" + message() + "]";
    };
};

#endif //MULTIPATH_PROXY_EXCEPTION_H
