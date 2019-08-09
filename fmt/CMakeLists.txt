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
    include/fmt/time.h
    include/fmt/chrono.h
    include/fmt/color.h
    include/fmt/locale.h)

set(fmt_SOURCES ${fmt_HEADERS}
    src/posix.cc
    src/format.cc)

add_library(fmtObjs OBJECT ${fmt_SOURCES})
target_include_directories(fmtObjs PUBLIC include)
target_compile_features(fmtObjs PUBLIC cxx_variadic_templates)

list(APPEND cppkit_OBJECTS $<TARGET_OBJECTS:fmtObjs>)
set(cppkit_OBJECTS ${cppkit_OBJECTS} PARENT_SCOPE)

add_library(fmt INTERFACE)
target_include_directories(fmt INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>)
target_compile_features(fmt INTERFACE cxx_variadic_templates)

install(FILES ${fmt_HEADERS} DESTINATION include/fmt)
