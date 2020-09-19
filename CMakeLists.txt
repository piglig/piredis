cmake_minimum_required (VERSION 2.6)
# specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

link_directories(
    /usr/local/lib
)

include_directories("/usr/local/include/hiredis")

add_executable(test test.cpp)

target_link_libraries(test -lhiredis)
