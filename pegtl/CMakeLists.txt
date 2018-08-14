#
# CMakeLists.txt
#

add_library(pegtl INTERFACE)
target_include_directories(pegtl INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>)

install(DIRECTORY tao DESTINATION include)
