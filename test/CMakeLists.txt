project(test_multipath_proxy)

# googletest
FetchContent_Declare(googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG        v1.13.0
        )# release-1.10.0)
FetchContent_MakeAvailable(googletest)

# uuid
# find_package(uuid REQUIRED)
# include_directories(${UUID_INCLUDE_DIRS})

# curses
#find_package(Curses REQUIRED)
#include_directories(${CURSES_INCLUDE_DIR})

#quiche
#include_directories(/root/quiche/quiche/include)
#link_directories(/root/quiche/target/debug)

file(GLOB TESTS *.cpp)
file(GLOB_RECURSE SRCS ../src/*.cpp ../src/*.h)
list(FILTER SRCS EXCLUDE REGEX ".*(main.cpp|main.h)$")
add_executable(test_multipath_proxy ${SRCS} ${TESTS})
target_link_libraries(test_multipath_proxy gtest glog)
# target_link_libraries(test_multipath_proxy ${UUID_LIBRARIES})
