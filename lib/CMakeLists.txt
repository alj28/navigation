
# Create a header-only library
add_library(cpp_lib INTERFACE)

find_library(GPS_LIB gps)
if(NOT GPS_LIB)
    message(FATAL_ERROR "libgps not found")
endif()
target_link_libraries(cpp_lib INTERFACE ${GPS_LIB})

find_library(LOG_LIB spdlog)
if(NOT LOG_LIB)
    message(FATA_ERROR "log library not found")
endif()
target_link_libraries(cpp_lib INTERFACE ${LOG_LIB})

find_library(FMT_LIB fmt)
if(NOT FMT_LIB)
    message(FATAL_ERROR "fmt library not found")
endif()
target_link_libraries(cpp_lib INTERFACE ${FMT_LIB})



include_directories(/usr/include)

# Add include directory
target_include_directories(cpp_lib
    INTERFACE
        .
)

add_subdirectory(gpsd)