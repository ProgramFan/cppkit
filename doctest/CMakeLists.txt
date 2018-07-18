#
# CMakeLists.txt
#

set(doctest_HEADERS doctest/doctest.h)
set(doctest_SOURCES ${doctest_HEADERS})
install(FILES ${doctest_HEADERS} DESTINATION include/doctest)
