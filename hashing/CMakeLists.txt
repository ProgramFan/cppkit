#
# Targets for the hashing library
#

add_library(
  hashing_OBJECTS OBJECT
  include/hashing/md5.h
  include/hashing/sha256.h
  include/hashing/blake3.h
  include/hashing/xxhash.h
  src/md5.c src/sha256.c sha/xxhash.c
  src/blake3.c src/blake3_dispatch.c src/blake3_portable.c
)

add_library(hashing INTERFACE)
target_include_directories(
  hashing INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)
