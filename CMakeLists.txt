cmake_minimum_required(VERSION 3.2)

project(TinySea LANGUAGES CXX)
enable_testing()

# Enable C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(TINY_SEA_BUILD_TESTS "Build tiny sea tests" TRUE)

include(${CMAKE_CURRENT_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)

add_subdirectory(src)

# Only enable tests if option is set and if gtest is require
if(TINY_SEA_BUILD_TESTS AND TARGET CONAN_PKG::gtest)
    add_subdirectory(tests)
endif()
