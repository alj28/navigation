cmake_minimum_required(VERSION 3.28)

# Project name and language
project(navigation_app VERSION 1.0 LANGUAGES CXX)

# Set C++ standard (optional but recommended)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_subdirectory(lib)
add_subdirectory(demos)
