cmake_minimum_required(VERSION 3.0.0)
project(test VERSION 0.1.0)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Debug)
include_directories(include)
add_subdirectory(src)
link_directories(lib)
add_executable(main main.cc)
target_link_libraries(main MySql mysqlclient MySqlPool)