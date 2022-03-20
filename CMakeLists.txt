#
# CMakeLists.txt
#

cmake_minimum_required(VERSION 3.5)
set(cppkit_VERSION "2.0.0")
project(cppkit VERSION ${cppkit_VERSION} LANGUAGES CXX C)

# Add a meta-library to use all of the libraries. When you embed cppkit in a
# project, set `cppkit_COMPONENTS` to enable whatever you want. The component
# name is exactly the directory name.
if(not cppkit_COMPONENTS)
  set(cppkit_COMPONENTS cppkit doctest fmt.v4)
endif()
foreach(c ${cppkit_COMPONENTS})
  add_subdirectory(${c})
endforeach()

add_library(cppkit INTERFACE)
target_link_libraries(cppkit INTERFACE ${cppkit_COMPONENTS})
