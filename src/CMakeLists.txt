file(GLOB_RECURSE SOURCES *.cpp)
file(GLOB_RECURSE HEADERS *.h)

add_library(tiny_sea ${SOURCES} ${HEADERS})
target_include_directories(tiny_sea PUBLIC ".")
target_link_libraries(tiny_sea CONAN_PKG::eigen)

install(TARGETS tiny_sea DESTINATION lib)
install(DIRECTORY tiny_sea/ DESTINATION include/tiny_sea FILES_MATCHING PATTERN "*.h")
