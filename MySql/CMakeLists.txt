cmake_minimum_required(VERSION 3.0.0)
project(test VERSION 0.1.0)
set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(src)
include_directories(include)
add_executable(test main.cc)
target_link_libraries(test MySql mysqlclient)