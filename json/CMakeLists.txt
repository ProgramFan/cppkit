add_library(nlohmann::json INTERFACE)
target_include_directories(
  nlohmann::json INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)
target_compile_features(nlohmann::json PUBLIC cxx_std_11)
