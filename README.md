# CppKit: A set of tools for efficient C++ programming

## Features

CppKit is a collection of library components for efficient C++ programming.
At the moment, it includes these components:

1. assert.hpp for contract programming
2. doctest for TDD
3. fmt for easy string formatting
4. pegtl for PEG parser generator

## Usage

CppKit can be used as a standalone library or a cmake subject. When used as a
standalone library, cppkit shall be compiled before usage. Use cmake to
compile and install cppkit to `PREFIX`, then add `$PREFIX/include` and
`$PREFIX/lib` to include and library path, and link with `-lcppkit`:

```bash
mkdir build && cd build
cmake -DCMAKE_INSTALL_PREFIX=$PREFIX ..
make && make install
```

When used as a cmake subproject, just put the source tree into `cppkit`
directory, add the `cppkit` directory to project, and use
`target_link_libraries(foo cppkit)` to refer to cppkit. When installing, the
cppkit artifacts will be installed.

For advanced users, each components can be extracted and add to the project's
source tree.

The components retain their normal usage, refer to their documents for
detailed information.

## Development

When adding a library, follow these steps:

1. Extract only the sources and headers of upstream project `foo`
2. Put the sources and headers in a separate directory `foo` and add `foo` in
   project's CMakeLists.txt
3. Create a README.md in `foo` to document how to upgrade the library
4. Create a CMakeLists.txt in `foo` to build the sources and install the
   headers.

The headers shall be installed into the original directory layout. The sources
shall be built into an object library `fooObjs`. The object library foo shall
be appended into the `cppkit_OBJECTS` list as `$<TARGET_OBJECTS:fooObjs>`, and
the variable shall be set to the parent scope. The interfaces shall be added
to the `foo` interface library and set the build and install time path
accordingly. See the `asrt` library for an detailed example.
