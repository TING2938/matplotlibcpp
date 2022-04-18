
# Building it
file(GLOB SRC_FILES *.cpp)
foreach(SRC_PATH ${SRC_FILES})
    get_filename_component(_target ${SRC_PATH} NAME_WE)
    add_executable(${_target} ${SRC_PATH})
    target_link_libraries(${_target} ${Python3_LIBRARIES} Python3::NumPy)
endforeach()

