add_library(fmt.v4 INTERFACE)
add_library(fmt::fmt.v4 ALIAS fmt.v4)
target_compile_definitions(fmt.v4 INTERFACE FMT_HEADER_ONLY=1)
target_include_directories(
  fmt.v4 INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)
