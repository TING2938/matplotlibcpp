project(matplotlibcpp)
cmake_minimum_required(VERSION 3.2)

set(CMAKE_CXX_STANDARD 11)

include_directories(${PROJECT_SOURCE_DIR}/include)

find_package(Python3 COMPONENTS NumPy Interpreter Development REQUIRED)
include_directories(${Python3_INCLUDE_DIRS})

add_subdirectory(demo)