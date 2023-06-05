cmake_minimum_required(VERSION 3.22)
project(multipath_proxy VERSION 0.1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# glog
#[[include(FetchContent)
FetchContent_Declare(
        glog
        GIT_REPOSITORY https://github.com/google/glog.git
        GIT_TAG v0.6.0
)
FetchContent_MakeAvailable(glog)]]

add_subdirectory(src)
add_subdirectory(test)