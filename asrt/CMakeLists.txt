#
# CMakeLists.txt
#

set(asrt_HEADERS asrt.h)
set(asrt_SOURCES ${asrt_HEADERS} asrt.cpp)

# Add the object library to cppkit
add_library(asrtObjs OBJECT ${asrt_SOURCES})
set(asrt_DEFS ASRT_ENABLE=1 ASRT_ENABLE_ASSERT_MACROS=1
    ASRT_ENABLE_CUSTOM_ERROR_HANDLER=1)
target_compile_definitions(asrtObjs PRIVATE ${asrt_DEFS})
list(APPEND cppkit_OBJECTS $<TARGET_OBJECTS:asrtObjs>)
set(cppkit_OBJECTS ${cppkit_OBJECTS} PARENT_SCOPE)

# Add the interface paths to cppkit
add_library(asrt INTERFACE)
target_include_directories(asrt INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>)

# Don't forget to install the interface files
install(FILES ${asrt_HEADERS} DESTINATION include)
