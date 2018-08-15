#
# CMakeLists.txt
#

cmake_minimum_required(VERSION 3.5)

set(asrt_VERSION 1.0.0)
project(asrt VERSION ${asrt_VERSION} LANGUAGES CXX)

add_library(asrt asrt.h asrt.cpp)
set(asrt_DEFS ASRT_ENABLE=1 ASRT_ENABLE_ASSERT_MACROS=1
    ASRT_ENABLE_CUSTOM_ERROR_HANDLER=1)
target_compile_definitions(asrt PRIVATE ${asrt_DEFS})
target_include_directories(asrt PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>)

install(TARGETS asrt EXPORT asrtTargets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    INCLUDES DESTINATION include)
install(EXPORT asrtTargets FILE asrtTargets.cmake
    NAMESPACE cppkit:: DESTINATION lib/cmake/cppkit)
