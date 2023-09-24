# curses
#find_package(Curses REQUIRED)
#include_directories(${CURSES_INCLUDE_DIR})

# quiche
#include_directories(/root/quiche/quiche/include)
#link_directories(/root/quiche/target/debug)
#message(${CMAKE_BINARY_DIR})
#file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/cert.crt DESTINATION ${CMAKE_BINARY_DIR}/src)
#file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/cert.key DESTINATION ${CMAKE_BINARY_DIR}/src)
#file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/rootca.crt DESTINATION ${CMAKE_BINARY_DIR}/src)

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

# uuid
# find_package(uuid REQUIRED)
# include_directories(${UUID_INCLUDE_DIRS})

#link_directories("/usr/local/lib")
file(GLOB_RECURSE SRCS "*.cpp" "*.h")
add_executable(mpp ${SRCS})
target_link_libraries(mpp glog::glog)
#target_link_libraries(mpp PRIVATE picoquic-log picoquic-core)
#target_include_directories(mpp PRIVATE picoquic)
# target_link_libraries(test_multipath_proxy ${UUID_LIBRARIES})