#
# CMakeLists.txt
#

cmake_minimum_required(VERSION 3.1)

set(MASTER_PROJECT OFF)
if ("${CMAKE_CURRENT_SOURCE_DIR}" STREQUAL "${CMAKE_SOURCE_DIR}")
  set(MASTER_PROJECT ON)
  message(STATUS "CMake version: ${CMAKE_VERSION}")
endif()

set(cppkit_VERSION "1.1.0")
project(cppkit VERSION ${cppkit_VERSION} LANGUAGES CXX C)

set(cppkit_COMPONENTS cppkit doctest fmt pegtl)
foreach (c ${cppkit_COMPONENTS})
  add_subdirectory(${c})
endforeach()
add_library(cppkit INTERFACE)
target_link_libraries(cppkit INTERFACE ${cppkit_COMPONENTS})

if (MASTER_PROJECT)
install(TARGETS cppkit EXPORT cppkitTargets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    INCLUDES DESTINATION include)
install(EXPORT cppkitTargets
    FILE cppkitTargets.cmake DESTINATION lib/cmake/cppkit)
endif()
