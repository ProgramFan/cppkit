#
# CMakeLists.txt
#

set(asrt_HEADERS asrt.h)
set(asrt_SOURCES ${asrt_HEADERS} asrt.cpp)

add_library(asrt OBJECT ${asrt_SOURCES})
set(asrt_DEFS ASRT_ENABLE=1 ASRT_ENABLE_ASSERT_MACROS=1
    ASRT_ENABLE_CUSTOM_ERROR_HANDLER=1)
set_target_properties(asrt PROPERTIES COMPILE_DEFINITIONS ${asrt_DEFS})
list(APPEND cppkit_OBJECTS $<TARGET_OBJECTS:asrt>)
set(cppkit_OBJECTS ${cppkit_OBJECTS} PARENT_SCOPE)

install(FILES ${asrt_HEADERS} DESTINATION include)
