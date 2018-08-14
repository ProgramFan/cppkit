#
# CMakeLists.txt
#

cmake_minimum_required(VERSION 3.0)

project(cppkit CXX C)

set(cppkit_VERSION "1.1.0")

set(cppkit_OBJECTS "")

add_subdirectory(asrt)
add_subdirectory(doctest)
add_subdirectory(fmt)
add_subdirectory(pegtl)

add_library(cppkit ${cppkit_OBJECTS})
install(TARGETS cppkit DESTINATION lib)
