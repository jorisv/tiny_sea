set(SOURCES tmp.cpp)
set(HEADERS strong_typedef.h regular_grid.h)

add_library(tiny_sea ${SOURCES} ${HEADERS})

install(TARGETS tiny_sea DESTINATION lib)
install (FILES ${HEADERS} DESTINATION include)
