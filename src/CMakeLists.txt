# glog
find_package(glog 0.6.0 REQUIRED)

# curses
find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})

## local
#file(GLOB_RECURSE LOCAL_SRCS "*.cpp" "*.h")
#list(FILTER LOCAL_SRCS EXCLUDE REGEX ".*(Remote.cpp|Remote.h)$")
#
#add_executable(local ${LOCAL_SRCS})
#target_link_libraries (local glog::glog ${CURSES_LIBRARIES})
#
## remote
#file(GLOB_RECURSE REMOTE_SRCS "*.cpp" "*.h")
#list(FILTER LOCAL_SRCS EXCLUDE REGEX ".*(Local.cpp|Local.h)$")
#
#add_executable(remote ${REMOTE_SRCS})
#target_link_libraries (remote glog::glog ${CURSES_LIBRARIES})

file(GLOB_RECURSE SRCS "*.cpp" "*.h")
add_executable(mpp ${SRCS})
target_link_libraries(mpp glog::glog ${CURSES_LIBRARIES})