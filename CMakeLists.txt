cmake_minimum_required(VERSION 3.22)
project(mpp VERSION 0.1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(CMakePrintHelpers)

# include FetchContent
include(FetchContent)
set(FETCHCONTENT_QUIET OFF)

# glog & gflags
FIND_PACKAGE(glog REQUIRED)
include_directories(${GLOG_INCLUDE_DIR})
#FetchContent_Declare(
#        gflags
#        GIT_REPOSITORY https://github.com/gflags/gflags.git
#        GIT_TAG master
#        )
#FetchContent_Declare(
#        glog
#        GIT_REPOSITORY https://github.com/google/glog.git
#        GIT_TAG v0.6.0
#)
#FetchContent_MakeAvailable(glog)

# picoquic
#set(PICOQUIC_FETCH_PTLS ON)
#include(FetchContent)
#FetchContent_Declare(
#        picoquic
#        GIT_REPOSITORY      https://github.com/private-octopus/picoquic.git
#        GIT_TAG             master)
#FETCHCONTENT_GETPROPERTIES(picoquic)
#if(NOT picoquic_POPULATED)
#    FETCHCONTENT_POPULATE(picoquic)
#    cmake_print_variables(picoquic_SOURCE_DIR)
#    include_directories(${picoquic_SOURCE_DIR}/picoquic ${picoquic_SOURCE_DIR}/loglib ${picoquic_SOURCE_DIR}/picohttp)
#    add_subdirectory(${picoquic_SOURCE_DIR})
#endif()
#FetchContent_MakeAvailable(picoquic)

find_package(OpenSSL REQUIRED)
if(OpenSSL_FOUND)
    include_directories(${OPENSSL_INCLUDE_DIRS})
    link_directories(${OPENSSL_LIBRARIES})
    message(STATUS "Using OpenSSL ${OPENSSL_VERSION}")
endif()

set(PICOQUIC_FETCH_PTLS 1)
add_subdirectory(third-party/picoquic)

add_subdirectory(src)
add_subdirectory(test)