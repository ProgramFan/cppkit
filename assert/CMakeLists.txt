#
# CMakeLists.txt
#

cmake_minimum_required(VERSION 3.5)

set(assert_VERSION 1.0.0)
project(assert VERSION ${assert_VERSION} LANGUAGES CXX)

add_library(assert assert.hpp assert.cpp)
set(assert_DEFS CPPKIT_ASSERT_ENABLE=1 CPPKIT_ASSERT_ENABLE_ASSERT_MACROS=1
    CPPKIT_ASSERT_ENABLE_CUSTOM_ERROR_HANDLER=1)
target_compile_definitions(assert PRIVATE ${assert_DEFS})
target_include_directories(assert PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>)

install(TARGETS assert EXPORT assertTargets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    INCLUDES DESTINATION include/cppkit)
install(EXPORT assertTargets FILE assertTargets.cmake
    NAMESPACE cppkit:: DESTINATION lib/cmake/cppkit)
