cmake_minimum_required(VERSION 3.0)
project(TIMER_LIBRARY)

# Set C++ standard to C++11
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Create a library from Timer.cpp
add_library(timer_lib SHARED src/Timer.cpp)

# Specify include directories for the library
target_include_directories(timer_lib PUBLIC inc)
