#
# CMakeLists.txt
#

cmake_minimum_required(VERSION 3.5)

set(assert_VERSION 1.1.0)
project(assert VERSION ${assert_VERSION} LANGUAGES CXX)

add_library(cppkit::assert include/cppkit/assert.hpp src/assert.cpp)
set(
  assert_DEFS CPPKIT_ASSERT_ENABLE=1 CPPKIT_ASSERT_ENABLE_ASSERT_MACROS=1
  CPPKIT_ASSERT_ENABLE_CUSTOM_ERROR_HANDLER=1
)
target_compile_definitions(cppkit::assert PRIVATE ${assert_DEFS})
target_include_directories(
  cppkit::assert PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)
