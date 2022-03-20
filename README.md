# CppKit: A set of tools for efficient C++ programming

## Features

CppKit is a collection of libraries for efficient C++ programming under C++11.
The gathered library is at best header-only, and only requires a C++11 compiler
or C99 compiler. Some big libraries such as googletest and abseil are also
included, and they are just the same as their origin.

## Usage

CppKit assumes it is embedded into another project, and allows the user to
choose whatever they need. To embed CppKit using cmake, one just need to put
this directory under a cmake-accessable directory and add
`add_subdirectory(cppkit)` to the parent CMakeLists.txt.

CppKit also assumes all its objects are embedded directly into the parent
project's binaries and libraries. Thus, it does not produce any extra libraries. Instead, the parent project shall embed cppkit's objects into its binaries and libraries.To do that, one add the following lines to the CMakeLists.txt:

```cmake
add_library(my-lib mysrc.c ${cppkit_OBJECTS})
add_executable(my-exe mysrc.c ${cppkit_OBJECTS})
target_link_libraries(my-lib PRIVATE cppkit)
target_link_libraries(my-exe PRIVATE cppkit)
```

## Development

Follow these steps to add a library:

1. Extract the sources and headers of upstream project `foo` into a directory
   `foo`.
2. Create a CMakeLists.txt in `foo` to build the library's source into an object
   library `foo_OBJECTS`. Then add a `foo` interface library for the headers.
   Finally, append `foo_OBJECTS` into the `cppkit_OBJECTS` list as
   `${TARGET_OBJECTS:foo_OBJECTS}`.
3. Create a README.md in `foo` to document how to upgrade the library


## Available Library Components

### fmt

The famous fmtlib which implements c++20 `std::format`. This library is
cross-platform and compatible with any c++11-compliant compilers, such as gcc
4.8 and visual studio 2015. It is configured as header-only.

### fmt.v4

This is a c++98 compatible version of the fmt library, with less features. But
if one sticks to the core features, it is useful. This is recommended for
projects under gcc 4.4 and visual studio 2010.

### spdlog

A fast C++11 logger with fmt as its message formatting interface.and header-only logger.

### googletest

The famous c++ unittest framework from google. This does not require C++11.

### doctest

The famous lightweight c++ unittest framework. The C++11 compatible version is
bundled.

### cppkit

A home-grown library for easy c++ programmng. Currently only `assert.hpp` for
straightforward contract programming.

### hashing

A collection of high quality hashing functions. Currently there are md5, sha256,
blake3 and xxhash.

### json

A header-only json parser and serializer.

### pegtl

A template library for PEG (parsing expression grammar).
