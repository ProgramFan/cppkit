#
# CMakeLists.txt
#

cmake_minimum_required(VERSION 3.5)

set(cppkit_VERSION "1.1.0")
project(cppkit VERSION ${cppkit_VERSION} LANGUAGES CXX C)

# set(cppkit_COMPONENTS asrt doctest fmt pegtl)
set(cppkit_COMPONENTS asrt)
foreach (c ${cppkit_COMPONENTS})
    add_subdirectory(${c})
endforeach()
add_library(cppkit INTERFACE)
target_link_libraries(cppkit INTERFACE ${cppkit_COMPONENTS})

install(TARGETS cppkit EXPORT cppkitTargets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    INCLUDES DESTINATION include)
install(EXPORT cppkitTargets
    FILE cppkitTargets.cmake DESTINATION lib/cmake/cppkit)
