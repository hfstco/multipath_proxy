project(test_multipath_proxy)

# gtest
find_package(GTest)

# glog
find_package(glog 0.4.0 REQUIRED)

# uuid
# find_package(uuid REQUIRED)
# include_directories(${UUID_INCLUDE_DIRS})

# curses
find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})

file(GLOB TESTS *.cpp)
file(GLOB_RECURSE SRCS ../src/*.cpp ../src/*.h)
list(FILTER SRCS EXCLUDE REGEX ".*(main.cpp|main.h)$")
add_executable(test_multipath_proxy ${SRCS} ${TESTS})
target_link_libraries(test_multipath_proxy GTest::gtest glog::glog ${CURSES_LIBRARIES})
# target_link_libraries(test_multipath_proxy ${UUID_LIBRARIES})
