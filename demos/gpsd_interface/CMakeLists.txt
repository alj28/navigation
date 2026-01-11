
set(CMAKE_BUILD_TYPE Debug)

add_executable(gpsd_interface
    src/main.cpp
)

target_link_libraries(gpsd_interface PRIVATE cpp_lib)
