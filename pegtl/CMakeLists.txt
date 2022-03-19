#
# CMakeLists.txt
#

# An interface library for a header-only library
add_library(pegtl INTERFACE)
target_include_directories(
  pegtl INTERFACE
  $<BUILD_INTERFACE:include>
  $<INSTALL_INTERFACE:include>
)

install(DIRECTORY include/tao DESTINATION include)
