//
// Created by Matthias Hofstätter on 23.02.23.
//

#ifndef MULTIPATH_PROXY_EXCEPTION_H
#define MULTIPATH_PROXY_EXCEPTION_H

#include <exception>
#include <string>


class Exception : std::exception {
public:
    Exception(const char *what_arg) : _message(what_arg) {};

    Exception(std::string what_arg) : _message(what_arg) {};

    const char *what() { return _message.c_str(); }

private:
    const std::string _message;
};

// dev exceptions

class NotImplementedError : public Exception {
public:
    NotImplementedError(const char *what_arg) : Exception(what_arg) {};

    NotImplementedError(std::string what_arg) : Exception(what_arg) {};
};

// net exceptions

class NetworkException : public Exception {
public:
    NetworkException(const char *what_arg) : Exception(what_arg) {};

    NetworkException(std::string what_arg) : Exception(what_arg) {};
};

class SocketErrorException : public Exception {
public:
    SocketErrorException(const char *what_arg) : Exception(what_arg) {};

    SocketErrorException(std::string what_arg) : Exception(what_arg) {};
};

class SocketClosedException: public Exception {
public:
    SocketClosedException(const char *what_arg) : Exception(what_arg) {};

    SocketClosedException(std::string what_arg) : Exception(what_arg) {};
};

class SockAddrErrorException : public Exception {
public:
    SockAddrErrorException(const char *what_arg) : Exception(what_arg) {};

    SockAddrErrorException(std::string what_arg) : Exception(what_arg) {};
};

// collections exception

class NotFoundException : public Exception {
public:
    NotFoundException(const char *what_arg) : Exception(what_arg) {};

    NotFoundException(std::string what_arg) : Exception(what_arg) {};
};

// utils exceptions

class UtilsException : public Exception {
public:
    UtilsException(const char *what_arg) : Exception(what_arg) {};

    UtilsException(std::string what_arg) : Exception(what_arg) {};
};

// flow exception

class ConnectionClosedException : public Exception {
public:
    ConnectionClosedException(const char *what_arg) : Exception(what_arg) {};

    ConnectionClosedException(std::string what_arg) : Exception(what_arg) {};
};

class GotClosePacketException : public Exception {
public:
    GotClosePacketException(const char *what_arg) : Exception(what_arg) {};

    GotClosePacketException(std::string what_arg) : Exception(what_arg) {};
};

// listener exception

class ListenerClosedException : public Exception {
public:
    ListenerClosedException(const char *what_arg) : Exception(what_arg) {};

    ListenerClosedException(std::string what_arg) : Exception(what_arg) {};
};

// bond exception

class SocketAlreadyConnectedException : public Exception {
public:
    SocketAlreadyConnectedException(const char *what_arg) : Exception(what_arg) {};

    SocketAlreadyConnectedException(std::string what_arg) : Exception(what_arg) {};
};

// buffer exception

class BufferReallocErrorException : public Exception {
public:
    BufferReallocErrorException(const char *what_arg) : Exception(what_arg) {};

    BufferReallocErrorException(std::string what_arg) : Exception(what_arg) {};
};

#endif //MULTIPATH_PROXY_EXCEPTION_H
