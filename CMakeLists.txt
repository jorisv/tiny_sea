cmake_minimum_required(VERSION 3.1)

project(TinySea VERSION 0.1.0 LANGUAGES CXX)

if(EXISTS "${CMAKE_BINARY_DIR}/conan_paths.cmake")
    include(${CMAKE_BINARY_DIR}/conan_paths.cmake)
endif()

# Enable C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory(src)
# add_subdirectory(tests)
