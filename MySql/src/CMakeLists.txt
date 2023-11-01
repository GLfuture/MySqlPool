cmake_minimum_required(VERSION 3.0.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ../../lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ../../lib)

include_directories(../include)
include_directories(/usr/include)
add_library(MySql STATIC MySql.cc)