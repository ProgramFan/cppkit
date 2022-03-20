add_library(spdlog INTERFACE)
target_include_directories(
  spdlog
  INTERFACE
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>"
  "$<INSTALL_INTERFACE:include>"
)
target_compile_features(spdlog PUBLIC cxx_std_11)
