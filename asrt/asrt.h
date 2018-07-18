//
// @file asrt.h
// @author Yang Zhang <yang_zhang@iapcm.ac.cn>, 2018
//
// # ASRT: An assertion implementation for contract programming.
//
// ASRT implements assertion macros for contract programming. This library
// provides `require`, `ensure` and `assert` for precondition, postcondition
// and invariance check. All these checks can be disabled at compile time to
// support production build. This library also provides a `check` macro which
// is not disabled together with the other macros (but can also be disabled at
// compile time).
//
// ## Features
//
// The difference of ASRT to standard assert is that ASRT evaluates the
// operands of the assert expression and output their values when the
// assertion fails. This is extremely useful for error tracing.
//
// By default, ASRT throws an std::logic_error when assertion fails. Users can
// register their own error handler for these different types of assert macros
// to customize the failure behavior.
//
// ASRT is very lightweight. It consists of only two files: `asrt.h` and
// `asrt.cpp`. If the default failure behavior is enough, users can set
// `ASRT_ENABLE_CUSTOM_ERROR_HANDLER` to `0` and uses only `asrt.h`.
//
// ## Usage
//
// Just include `asrt.h` and call
// `<ASSERT|REQUIRE|ENSURE|CHECK>_<EQ|NE|GT|GE|LT|LE|TRUE|FALSE>` in the code
// to declare assertions. Add `asrt.cpp` to the build system if customization
// of error handlers are used.
//
// ## Customizations
//
// ASRT is higly customizable by providing a hierarchy of configuration
// macros.
//
// 1. set `ASRT_ENABLE` to `0` to disable all macros. Default to `1`.
// 2. set `ASRT_ENABLE_<ASSERT|REQUIRE|ENSURE|CHECK>_MACROS` to `0` to disable
// corresponding macros. By default, the `ASSERT` macros is controled by
// `NDEBUG` like the standard assert macro, the `REQUIRE` and `ENSURE` macros
// follow the setting of `ASSERT`, and the `CHECK` macro is enabled.
// 3. set `ASSERT_ENABLE_SHORT_MACROS` to `0` to disable short macros. Default
// to `1` if no name collision is found.
//
// Users can customize the error handler by defining function with the
// following signature and register to the system with
// `asrt::ErrorHandling::pushHandler(asrt::CHAN_ASSERT, handler)`.
//
// ```cpp
// void ErrorHandler(const std::string &file, int line,
//                   const std::string &raw_expr,
//                   const std::string &eval_expr);
// ```

#ifndef ASRT_H
#define ASRT_H

#define ASRT_VERSION_MAJOR 1
#define ASRT_VERSION_MINOR 0
#define ASRT_VERSION_PATCH 0
#define ASRT_VERSION_STRING "1.0.0"

// ==========================================================================
// Customization macros and their default values
// ==========================================================================

// Determine whether to enable the whole asrt implementation, default to 1.
#ifndef ASRT_ENABLE
#define ASRT_ENABLE 1
#endif

// Determine whether to enable check macros, default to 1
#ifndef ASRT_ENABLE_CHECK_MACROS
#define ASRT_ENABLE_CHECK_MACROS 1
#endif
// Determine whether to enable assertion macros, default to that of assert.
#ifndef ASRT_ENABLE_ASSERT_MACROS
#ifdef NDEBUG
#define ASRT_ENABLE_ASSERT_MACROS 0
#else
#define ASRT_ENABLE_ASSERT_MACROS 1
#endif
#endif
// Determine whether to enable require and ensure macros, default to that of
// assertion macros.
#ifndef ASRT_ENABLE_REQUIRE_MACROS
#define ASRT_ENABLE_REQUIRE_MACROS ASRT_ENABLE_ASSERT_MACROS
#endif
#ifndef ASRT_ENABLE_ENSURE_MACROS
#define ASRT_ENABLE_ENSURE_MACROS ASRT_ENABLE_ASSERT_MACROS
#endif
// Determine whether to enable short macros. By default, they are enabled when
// there exists no name collisions.
#ifndef ASRT_ENABLE_SHORT_MACROS
#if defined(ASSERT_EQ) || defined(ASSERT_NE) || defined(ASSERT_GT) ||    \
    defined(ASSERT_GE) || defined(ASSERT_LT) || defined(ASSERT_LE) ||    \
    defined(CHECK_EQ) || defined(CHECK_NE) || defined(CHECK_GT) ||       \
    defined(CHECK_GE) || defined(CHECK_LT) || defined(CHECK_LE) ||       \
    defined(REQUIRE_EQ) || defined(REQUIRE_NE) || defined(REQUIRE_GT) || \
    defined(REQUIRE_GE) || defined(REQUIRE_LT) || defined(REQUIRE_LE) || \
    defined(ENSURE_EQ) || defined(ENSURE_NE) || defined(ENSURE_GT) ||    \
    defined(ENSURE_GE) || defined(ENSURE_LT) || defined(ENSURE_LE)
#define ASRT_ENABLE_SHORT_MACROS 0
#else
#define ASRT_ENABLE_SHORT_MACROS 1
#endif
#endif
// Determine whether to enable custom error handler, default to 1.
#ifndef ASRT_ENABLE_CUSTOM_ERROR_HANDLER
#define ASRT_ENABLE_CUSTOM_ERROR_HANDLER 1
#endif

// ==========================================================================
// Customization interface implementation
// ==========================================================================

#include <sstream>  // required by make_eval_expr
#include <string>   // required by std::string

namespace asrt {
enum { CHAN_ASSERT = 0, CHAN_CHECK, CHAN_REQUIRE, CHAN_ENSURE, CHAN_COUNT_ };
class ErrorHandling {
public:
  typedef void (*ErrorHandler)(const std::string &file, int line,
                               const std::string &raw_expr,
                               const std::string &eval_expr);
#if ASRT_ENABLE == 1 && ASRT_ENABLE_CUSTOM_ERROR_HANDLER == 1
  static void pushHandler(int channel, ErrorHandler handler);
  static ErrorHandler popHandler(int channel);
  static void resetHandler(int channel, ErrorHandler handler = NULL);
  // Internal function used by binary_assert
  static void handleError_(int channel, const std::string &file, int line,
                           const std::string &raw_expr,
                           const std::string &eval_expr);
#else
  static void pushHandler(int channel, ErrorHandler handler) {}
  static ErrorHandler popHandler(int channel) { return NULL; }
  static void resetHandler(int channel, ErrorHandler handler = NULL) {}
#if ASRT_ENABLE == 1
  // Internal function used by binary_assert
  static void handleError_(int channel, const std::string &file, int line,
                           const std::string &raw_expr,
                           const std::string &eval_expr) {
    static std::string channel_names[CHAN_COUNT_] = {"assert", "check",
                                                     "require", "ensure"};
    std::ostringstream oss;
    oss << file << ":" << line << ": " << channel_names[channel] << "("
        << raw_expr << ") failed, values (" << eval_expr << ")";
    throw std::logic_error(oss.str());
  }
#endif
#endif
};
}  // namespace asrt

// ==========================================================================
// Macro implementation
// ==========================================================================

#if ASRT_ENABLE == 1

// Implementation details
namespace asrt {
namespace detail {

//
// To ensure the agruments are evaluated only once, one has to store them as
// temporaries in case the assertion fails. Since the macro arguments can have
// arbitrary types, auto keyword is required to define the temporaries. This
// will require C++11 and hurt the portability on C++98 compilers. We use
// another approach here (stolen from the famous doctest.h): We uses function
// templates to hide the real argument type all the way down to the assertion
// evaluation.
//

const int ASRT_CMP_EQ = 0;
const int ASRT_CMP_NE = 1;
const int ASRT_CMP_GT = 2;
const int ASRT_CMP_GE = 3;
const int ASRT_CMP_LT = 4;
const int ASRT_CMP_LE = 5;

template <int OP>
struct RelationalComparision {
  // Check if `x OP y` is valid
  template <typename L, typename R>
  static bool valid(const L &x, const R &y) {
    return false;
  }
  // Build a string representation of the comparision operator
  static std::string str() { return "?"; }
};
template <>
struct RelationalComparision<ASRT_CMP_EQ> {
  template <typename L, typename R>
  static bool valid(const L &x, const R &y) {
    return x == y;
  }
  static std::string str() { return "=="; }
};
template <>
struct RelationalComparision<ASRT_CMP_NE> {
  template <typename L, typename R>
  static bool valid(const L &x, const R &y) {
    return x != y;
  }
  static std::string str() { return "!="; }
};
template <>
struct RelationalComparision<ASRT_CMP_GT> {
  template <typename L, typename R>
  static bool valid(const L &x, const R &y) {
    return x > y;
  }
  static std::string str() { return ">"; }
};
template <>
struct RelationalComparision<ASRT_CMP_GE> {
  template <typename L, typename R>
  static bool valid(const L &x, const R &y) {
    return x >= y;
  }
  static std::string str() { return ">="; }
};
template <>
struct RelationalComparision<ASRT_CMP_LT> {
  template <typename L, typename R>
  static bool valid(const L &x, const R &y) {
    return x < y;
  }
  static std::string str() { return "<"; }
};
template <>
struct RelationalComparision<ASRT_CMP_LE> {
  template <typename L, typename R>
  static bool valid(const L &x, const R &y) {
    return x <= y;
  }
  static std::string str() { return "<="; }
};

template <typename T1, typename T2>
struct Result {
  Result(T1 x_, T2 y_) : x(x_), y(y_) {}
  T1 x;
  T2 y;
};
template <typename T1, typename T2>
inline Result<T1, T2> make_result(T1 x, T2 y) {
  return Result<T1, T2>(x, y);
}

template <int OP>
inline std::string make_expr_str(const char *x, const char *y) {
  return std::string(x) + " " + RelationalComparision<OP>::str() + " " + y;
}
template <int OP, typename L, typename R>
inline std::string make_eval_str(const L &x, const R &y) {
  std::ostringstream oss;
  oss << x << " " << RelationalComparision<OP>::str() << " " << y;
  return oss.str();
}

template <int OP, typename L, typename R>
inline void binary_assert(int channel, const char *file, int line,
                          const std::string &expr, const Result<L, R> &result) {
  if (RelationalComparision<OP>::valid(result.x, result.y)) return;
  asrt::ErrorHandling::handleError_(channel, file, line, expr,
                                    make_eval_str<OP>(result.x, result.y));
}

}  // namespace detail
}  // namespace asrt

// ==========================================================================
// Macro definitions
// ==========================================================================

#define ASRT_CHAN_ASSERT_IMPL(CHAN, OP, x, y)                           \
  asrt::detail::binary_assert<asrt::detail::ASRT_CMP_##OP>(             \
      CHAN, __FILE__, __LINE__,                                         \
      asrt::detail::make_expr_str<asrt::detail::ASRT_CMP_##OP>(#x, #y), \
      asrt::detail::make_result((x), (y)))
#define ASRT_CHAN_ASSERT_EQ(CHAN, x, y) ASRT_CHAN_ASSERT_IMPL(CHAN, EQ, x, y)
#define ASRT_CHAN_ASSERT_NE(CHAN, x, y) ASRT_CHAN_ASSERT_IMPL(CHAN, NE, x, y)
#define ASRT_CHAN_ASSERT_GT(CHAN, x, y) ASRT_CHAN_ASSERT_IMPL(CHAN, GT, x, y)
#define ASRT_CHAN_ASSERT_GE(CHAN, x, y) ASRT_CHAN_ASSERT_IMPL(CHAN, GE, x, y)
#define ASRT_CHAN_ASSERT_LT(CHAN, x, y) ASRT_CHAN_ASSERT_IMPL(CHAN, LT, x, y)
#define ASRT_CHAN_ASSERT_LE(CHAN, x, y) ASRT_CHAN_ASSERT_IMPL(CHAN, LE, x, y)
#define ASRT_CHAN_ASSERT_TRUE(CHAN, x) ASRT_CHAN_ASSERT_IMPL(CHAN, EQ, x, true)
#define ASRT_CHAN_ASSERT_FALSE(CHAN, x) \
  ASRT_CHAN_ASSERT_IMPL(CHAN, EQ, x, false)
#else
#define ASRT_CHAN_ASSERT_EQ(CHAN, x, y)
#define ASRT_CHAN_ASSERT_NE(CHAN, x, y)
#define ASRT_CHAN_ASSERT_GT(CHAN, x, y)
#define ASRT_CHAN_ASSERT_GE(CHAN, x, y)
#define ASRT_CHAN_ASSERT_LT(CHAN, x, y)
#define ASRT_CHAN_ASSERT_LE(CHAN, x, y)
#define ASRT_CHAN_ASSERT_TRUE(CHAN, x)
#define ASRT_CHAN_ASSERT_FALSE(CHAN, x)
#endif

#define ASRT_DO_ASSERT2(CH, OP, x, y) \
  ASRT_CHAN_ASSERT_##OP(asrt::CHAN_##CH, x, y)
#define ASRT_DO_ASSERT1(CH, OP, x) ASRT_CHAN_ASSERT_##OP(asrt::CHAN_##CH, x)

// The assertion macros
#if ASRT_ENABLE_ASSERT_MACROS == 1
#define ASRT_ASSERT_EQ(x, y) ASRT_DO_ASSERT2(ASSERT, EQ, x, y)
#define ASRT_ASSERT_NE(x, y) ASRT_DO_ASSERT2(ASSERT, NE, x, y)
#define ASRT_ASSERT_GT(x, y) ASRT_DO_ASSERT2(ASSERT, GT, x, y)
#define ASRT_ASSERT_GE(x, y) ASRT_DO_ASSERT2(ASSERT, GE, x, y)
#define ASRT_ASSERT_LT(x, y) ASRT_DO_ASSERT2(ASSERT, LT, x, y)
#define ASRT_ASSERT_LE(x, y) ASRT_DO_ASSERT2(ASSERT, LE, x, y)
#define ASRT_ASSERT_TRUE(x) ASRT_DO_ASSERT1(ASSERT, TRUE, x)
#define ASRT_ASSERT_FALSE(x) ASRT_DO_ASSERT1(ASSERT, FALSE, x)
#else
#define ASRT_ASSERT_EQ(x, y)
#define ASRT_ASSERT_NE(x, y)
#define ASRT_ASSERT_GT(x, y)
#define ASRT_ASSERT_GE(x, y)
#define ASRT_ASSERT_LT(x, y)
#define ASRT_ASSERT_LE(x, y)
#define ASRT_ASSERT_TRUE(x)
#define ASRT_ASSERT_FALSE(x)
#endif

// The check macros
#if ASRT_ENABLE_CHECK_MACROS == 1
#define ASRT_CHECK_EQ(x, y) ASRT_DO_ASSERT2(CHECK, EQ, x, y)
#define ASRT_CHECK_NE(x, y) ASRT_DO_ASSERT2(CHECK, NE, x, y)
#define ASRT_CHECK_GT(x, y) ASRT_DO_ASSERT2(CHECK, GT, x, y)
#define ASRT_CHECK_GE(x, y) ASRT_DO_ASSERT2(CHECK, GE, x, y)
#define ASRT_CHECK_LT(x, y) ASRT_DO_ASSERT2(CHECK, LT, x, y)
#define ASRT_CHECK_LE(x, y) ASRT_DO_ASSERT2(CHECK, LE, x, y)
#define ASRT_CHECK_TRUE(x) ASRT_DO_ASSERT1(CHECK, TRUE, x)
#define ASRT_CHECK_FALSE(x) ASRT_DO_ASSERT1(CHECK, FALSE, x)
#else
#define ASRT_CHECK_EQ(x, y)
#define ASRT_CHECK_NE(x, y)
#define ASRT_CHECK_GT(x, y)
#define ASRT_CHECK_GE(x, y)
#define ASRT_CHECK_LT(x, y)
#define ASRT_CHECK_LE(x, y)
#define ASRT_CHECK_TRUE(x)
#define ASRT_CHECK_FALSE(x)
#endif

// The require macros
#if ASRT_ENABLE_REQUIRE_MACROS == 1
#define ASRT_REQUIRE_EQ(x, y) ASRT_DO_ASSERT2(REQUIRE, EQ, x, y)
#define ASRT_REQUIRE_NE(x, y) ASRT_DO_ASSERT2(REQUIRE, NE, x, y)
#define ASRT_REQUIRE_GT(x, y) ASRT_DO_ASSERT2(REQUIRE, GT, x, y)
#define ASRT_REQUIRE_GE(x, y) ASRT_DO_ASSERT2(REQUIRE, GE, x, y)
#define ASRT_REQUIRE_LT(x, y) ASRT_DO_ASSERT2(REQUIRE, LT, x, y)
#define ASRT_REQUIRE_LE(x, y) ASRT_DO_ASSERT2(REQUIRE, LE, x, y)
#define ASRT_REQUIRE_TRUE(x) ASRT_DO_ASSERT1(REQUIRE, TRUE, x)
#define ASRT_REQUIRE_FALSE(x) ASRT_DO_ASSERT1(REQUIRE, FALSE, x)
#else
#define ASRT_REQUIRE_EQ(x, y)
#define ASRT_REQUIRE_NE(x, y)
#define ASRT_REQUIRE_GT(x, y)
#define ASRT_REQUIRE_GE(x, y)
#define ASRT_REQUIRE_LT(x, y)
#define ASRT_REQUIRE_LE(x, y)
#define ASRT_REQUIRE_TRUE(x)
#define ASRT_REQUIRE_FALSE(x)
#endif

// The ensure macros
#if ASRT_ENABLE_ENSURE_MACROS == 1
#define ASRT_ENSURE_EQ(x, y) ASRT_DO_ASSERT2(ENSURE, EQ, x, y)
#define ASRT_ENSURE_NE(x, y) ASRT_DO_ASSERT2(ENSURE, NE, x, y)
#define ASRT_ENSURE_GT(x, y) ASRT_DO_ASSERT2(ENSURE, GT, x, y)
#define ASRT_ENSURE_GE(x, y) ASRT_DO_ASSERT2(ENSURE, GE, x, y)
#define ASRT_ENSURE_LT(x, y) ASRT_DO_ASSERT2(ENSURE, LT, x, y)
#define ASRT_ENSURE_LE(x, y) ASRT_DO_ASSERT2(ENSURE, LE, x, y)
#define ASRT_ENSURE_TRUE(x) ASRT_DO_ASSERT1(ENSURE, TRUE, x)
#define ASRT_ENSURE_FALSE(x) ASRT_DO_ASSERT1(ENSURE, FALSE, x)
#else
#define ASRT_ENSURE_EQ(x, y)
#define ASRT_ENSURE_NE(x, y)
#define ASRT_ENSURE_GT(x, y)
#define ASRT_ENSURE_GE(x, y)
#define ASRT_ENSURE_LT(x, y)
#define ASRT_ENSURE_LE(x, y)
#define ASRT_ENSURE_TRUE(x)
#define ASRT_ENSURE_FALSE(x)
#endif

// ==========================================================================
// Short macros
// ==========================================================================

#if ASRT_ENABLE_SHORT_MACROS == 1
#define ASSERT_EQ(x, y) ASRT_ASSERT_EQ(x, y)
#define ASSERT_NE(x, y) ASRT_ASSERT_NE(x, y)
#define ASSERT_GT(x, y) ASRT_ASSERT_GT(x, y)
#define ASSERT_GE(x, y) ASRT_ASSERT_GE(x, y)
#define ASSERT_LT(x, y) ASRT_ASSERT_LT(x, y)
#define ASSERT_LE(x, y) ASRT_ASSERT_LE(x, y)
#define ASSERT_TRUE(x) ASRT_ASSERT_TRUE(x)
#define ASSERT_FALSE(x) ASRT_ASSERT_FALSE(x)
#define CHECK_EQ(x, y) ASRT_CHECK_EQ(x, y)
#define CHECK_NE(x, y) ASRT_CHECK_NE(x, y)
#define CHECK_GT(x, y) ASRT_CHECK_GT(x, y)
#define CHECK_GE(x, y) ASRT_CHECK_GE(x, y)
#define CHECK_LT(x, y) ASRT_CHECK_LT(x, y)
#define CHECK_LE(x, y) ASRT_CHECK_LE(x, y)
#define CHECK_TRUE(x) ASRT_CHECK_TRUE(x)
#define CHECK_FALSE(x) ASRT_CHECK_FALSE(x)
#define REQUIRE_EQ(x, y) ASRT_REQUIRE_EQ(x, y)
#define REQUIRE_NE(x, y) ASRT_REQUIRE_NE(x, y)
#define REQUIRE_GT(x, y) ASRT_REQUIRE_GT(x, y)
#define REQUIRE_GE(x, y) ASRT_REQUIRE_GE(x, y)
#define REQUIRE_LT(x, y) ASRT_REQUIRE_LT(x, y)
#define REQUIRE_LE(x, y) ASRT_REQUIRE_LE(x, y)
#define REQUIRE_TRUE(x) ASRT_REQUIRE_TRUE(x)
#define REQUIRE_FALSE(x) ASRT_REQUIRE_FALSE(x)
#define ENSURE_EQ(x, y) ASRT_ENSURE_EQ(x, y)
#define ENSURE_NE(x, y) ASRT_ENSURE_NE(x, y)
#define ENSURE_GT(x, y) ASRT_ENSURE_GT(x, y)
#define ENSURE_GE(x, y) ASRT_ENSURE_GE(x, y)
#define ENSURE_LT(x, y) ASRT_ENSURE_LT(x, y)
#define ENSURE_LE(x, y) ASRT_ENSURE_LE(x, y)
#define ENSURE_TRUE(x) ASRT_ENSURE_TRUE(x)
#define ENSURE_FALSE(x) ASRT_ENSURE_FALSE(x)
#endif

#endif  // ASRT_H
