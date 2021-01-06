cmake_minimum_required(VERSION 3.17)  # CMake version check
project(project)               # Create project "simple_example"
set(CMAKE_CXX_STANDARD 20)            # Enable c++14 standard

# Add main.cpp file of project root directory as source file
set(SOURCE_FILES
        src/main.cpp
        inc/MyClass.h
        inc/serialize/archive.hpp
        inc/serialize/iarchive.hpp
        inc/serialize/oarchive.hpp
        inc/serialize/stream.hpp
        )

# Add executable target with source files listed in SOURCE_FILES variable
add_executable(project ${SOURCE_FILES})
