#
# CMakeLists.txt
#

set(fmt_HEADERS
    include/fmt/core.h
    include/fmt/format.h
    include/fmt/format-inl.h
    include/fmt/ostream.h
    include/fmt/posix.h
    include/fmt/printf.h
    include/fmt/ranges.h
    include/fmt/time.h)

set(fmt_SOURCES ${fmt_HEADERS}
    src/posix.cc
    src/format.cc)

add_library(fmt OBJECT ${fmt_SOURCES})
target_include_directories(fmt PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
list(APPEND cppkit_OBJECTS $<TARGET_OBJECTS:fmt>)
set(cppkit_OBJECTS ${cppkit_OBJECTS} PARENT_SCOPE)

install(FILES ${fmt_HEADERS} DESTINATION include/fmt)
