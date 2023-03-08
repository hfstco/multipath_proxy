# glog
find_package(glog 0.6.0 REQUIRED)

# curses
find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})

file(GLOB_RECURSE SRCS "*.cpp" "*.h")

# left
add_executable(left ${SRCS})
target_link_libraries (left glog::glog ${CURSES_LIBRARIES})

# right
add_executable(right ${SRCS})
target_link_libraries (right glog::glog ${CURSES_LIBRARIES})