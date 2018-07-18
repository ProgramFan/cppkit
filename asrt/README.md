# ASRT: An assertion implementation for contract programming.

ASRT implements assertion macros for contract programming. This library
provides `require`, `ensure` and `assert` for precondition, postcondition
and invariance check. All these checks can be disabled at compile time to
support production build. This library also provides a `check` macro which
is not disabled together with the other macros (but can also be disabled at
compile time).

## Features

The difference of ASRT to standard assert is that ASRT evaluates the
operands of the assert expression and output their values when the
assertion fails. This is extremely useful for error tracing.

By default, ASRT throws an std::logic_error when assertion fails. Users can
register their own error handler for these different types of assert macros
to customize the failure behavior.

ASRT is very lightweight. It consists of only two files: `asrt.h` and
`asrt.cpp`. If the default failure behavior is enough, users can set
`ASRT_ENABLE_CUSTOM_ERROR_HANDLER` to `0` and uses only `asrt.h`.

## Usage

Just include `asrt.h` and call
`<ASSERT|REQUIRE|ENSURE|CHECK>_<EQ|NE|GT|GE|LT|LE|TRUE|FALSE>` in the code
to declare assertions. Add `asrt.cpp` to the build system if customization
of error handlers are used.

## Customizations

ASRT is higly customizable by providing a hierarchy of configuration
macros.

1. set `ASRT_ENABLE` to `0` to disable all macros. Default to `1`.
2. set `ASRT_ENABLE_<ASSERT|REQUIRE|ENSURE|CHECK>_MACROS` to `0` to disable
corresponding macros. By default, the `ASSERT` macros is controled by
`NDEBUG` like the standard assert macro, the `REQUIRE` and `ENSURE` macros
follow the setting of `ASSERT`, and the `CHECK` macro is enabled.
3. set `ASSERT_ENABLE_SHORT_MACROS` to `0` to disable short macros. Default
to `1` if no name collision is found.

Users can customize the error handler by defining function with the
following signature and register to the system with
`asrt::ErrorHandling::pushHandler(asrt::CHAN_ASSERT, handler)`.

```cpp
void ErrorHandler(const std::string &file, int line,
                  const std::string &raw_expr,
                  const std::string &eval_expr);
```

