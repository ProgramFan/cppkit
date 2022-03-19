add_library(fmt INTERFACE)
add_library(fmt::fmt ALIAS fmt)
target_compile_definitions(fmt INTERFACE FMT_HEADER_ONLY=1)
target_include_directories(
  fmt INTERFACE
  $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}>
  $<INSTALL_INTERFACE:include>
)
