#
# CMakeLists.txt
#

cmake_minimum_required(VERSION 3.0)

project(cppkit CXX C)

set(cppkit_VERSION "1.1.0")

# cppkit_OBJECTS are used to aggragates object files of cppkit library while
# cppkit_COMPONENTS are used to aggregates include paths.
set(cppkit_OBJECTS "")
set(cppkit_COMPONENTS asrt doctest fmt pegtl)

foreach (c ${cppkit_COMPONENTS})
    add_subdirectory(${c})
endforeach()

add_library(cppkit ${cppkit_OBJECTS})
target_link_libraries(cppkit ${cppkit_COMPONENTS})

install(TARGETS cppkit DESTINATION lib)
