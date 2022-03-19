#
# CMakeLists.txt
#

# Add an interface library for others to use our headers.
add_library(fmt INTERFACE)
target_include_directories(
  fmt INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
  )
target_compile_features(fmt INTERFACE cxx_std_11)
target_compile_definitions(fmt INTERFACE FMT_HEADER_ONLY)
add_library(fmt::fmt ALIAS fmt)
