add_library(cli11 INTERFACE)
target_include_directories(
  cli11 INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)
target_compile_features(cli11 PUBLIC cxx_std_11)
