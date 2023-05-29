cmake_minimum_required(VERSION 3.0.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ../../lib)

include_directories(/usr/include)
include_directories(../include)
add_library(MySqlPool SHARED MySqlPool.cc)