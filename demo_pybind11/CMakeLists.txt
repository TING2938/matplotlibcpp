cmake_minimum_required(VERSION 3.11)
project(demo_pybind11)

include(FetchContent)

FetchContent_Declare(
    pybind11
    URL https://github.com/pybind/pybind11/archive/refs/tags/v2.10.0.tar.gz
)
FetchContent_MakeAvailable(pybind11)

add_executable(demo_pybind11 demo_pybind11.cpp)
target_link_libraries(demo_pybind11 pybind11::embed)