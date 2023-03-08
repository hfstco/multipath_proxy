//
// Created by Matthias Hofstätter on 23.02.23.
//

#ifndef MULTIPATH_PROXY_EXCEPTION_H
#define MULTIPATH_PROXY_EXCEPTION_H

#include <exception>
#include <string>

namespace mpp {

    class Exception : std::exception {
    public:
        const char* what() { return _message; }
        
    protected:
        Exception(const char* what_arg) : _message(what_arg) {};
        Exception(std::string what_arg) : _message(what_arg.c_str()) {};

    private:
        const char* _message;
    };

    class NotImplementedError : public mpp::Exception {
    public:
        NotImplementedError(const char* what_arg) : mpp::Exception(what_arg) {};
        NotImplementedError(std::string what_arg) : mpp::Exception(what_arg) {};
    };

    class NetworkError : public mpp::Exception {
    public:
        NetworkError(const char* what_arg) : mpp::Exception(what_arg) {};
        NetworkError(std::string what_arg) : mpp::Exception(what_arg) {};
    };

    class SocketError : public mpp::Exception {
    public:
        SocketError(const char* what_arg) : mpp::Exception(what_arg) {};
        SocketError(std::string what_arg) : mpp::Exception(what_arg) {};
    };

    class SockAddrError : public mpp::Exception {
    public:
        SockAddrError(const char* what_arg) : mpp::Exception(what_arg) {};
        SockAddrError(std::string what_arg) : mpp::Exception(what_arg) {};
    };
}


#endif //MULTIPATH_PROXY_EXCEPTION_H
