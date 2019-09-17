file(GLOB_RECURSE SOURCES TEST_*.cpp)
set(HEADERS )

add_executable(tiny_sea_test ${SOURCES} ${HEADERS})
target_link_libraries(tiny_sea_test tiny_sea CONAN_PKG::gtest)
add_test(NAME tiny_sea_test_unit
         WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
         COMMAND tiny_sea_test)
