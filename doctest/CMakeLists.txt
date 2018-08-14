#
# CMakeLists.txt
#

set(doctest_HEADERS doctest/doctest.h)
set(doctest_SOURCES ${doctest_HEADERS})

# Header-only libraries only needs the interface library
add_library(doctest INTERFACE)
target_include_directories(doctest INTERFACE 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>)

install(FILES ${doctest_HEADERS} DESTINATION include/doctest)
