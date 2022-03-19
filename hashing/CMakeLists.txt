#
# Targets for the hashing library
#

add_library(hashing::md5 OBJECT md5.h md5.c)
target_include_directories(hashing::md5 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_library(hashing::sha256 OBJECT sha256.h sha256.c)
target_include_directories(hashing::sha256 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_library(hashing::blake3 OBJECT blake3.h blake3.c blake3_impl.h blake3_dispatch.c blake3_portable.c)
target_include_directories(hashing::blake3 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_library(hashing::xxhash OBJECT xxhash.h xxhash.c)
target_include_directories(hashing::xxhash PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
