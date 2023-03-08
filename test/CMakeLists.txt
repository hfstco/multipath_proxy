project(test_multipath_proxy)

# gtest
find_package(GTest)

# glog
find_package(glog 0.6.0 REQUIRED)

# curses
find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})

file(GLOB TESTS *.cpp)
file(GLOB_RECURSE SRCS ../src/*.cpp ../src/*.h)
add_executable(test_multipath_proxy ${SRCS} ${TESTS})
target_link_libraries (test_multipath_proxy GTest::gtest glog::glog ${CURSES_LIBRARIES})
