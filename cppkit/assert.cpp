#include "assert.hpp"

#if CPPKIT_ASSERT_ENABLE == 1 && CPPKIT_ASSERT_ENABLE_CUSTOM_ERROR_HANDLER == 1

#include <cassert>
#include <cstdio>
#include <stdexcept>
#include <vector>

namespace {
void handle_channel_error(const std::string &channel_name,
                          const std::string &file, int line,
                          const std::string &raw_expr,
                          const std::string &eval_expr) {
  char msg[1024] = {'\0'};
  snprintf(msg, 1023, "%s:%d: %s(%s) failed, values (%s)", file.c_str(), line,
           channel_name.c_str(), raw_expr.c_str(), eval_expr.c_str());
  throw std::logic_error(msg);
}

void handle_assert_error(const std::string &file, int line,
                         const std::string &raw_expr,
                         const std::string &eval_expr) {
  handle_channel_error("assert", file, line, raw_expr, eval_expr);
}
void handle_require_error(const std::string &file, int line,
                          const std::string &raw_expr,
                          const std::string &eval_expr) {
  handle_channel_error("require", file, line, raw_expr, eval_expr);
}
void handle_ensure_error(const std::string &file, int line,
                         const std::string &raw_expr,
                         const std::string &eval_expr) {
  handle_channel_error("ensure", file, line, raw_expr, eval_expr);
}
void handle_check_error(const std::string &file, int line,
                        const std::string &raw_expr,
                        const std::string &eval_expr) {
  handle_channel_error("check", file, line, raw_expr, eval_expr);
}

std::vector<cppkit::ErrorHandling::ErrorHandler>
    handlers_[cppkit::CHAN_COUNT_] = {
        std::vector<cppkit::ErrorHandling::ErrorHandler>(1,
                                                         handle_assert_error),
        std::vector<cppkit::ErrorHandling::ErrorHandler>(1, handle_check_error),
        std::vector<cppkit::ErrorHandling::ErrorHandler>(1,
                                                         handle_require_error),
        std::vector<cppkit::ErrorHandling::ErrorHandler>(1,
                                                         handle_ensure_error)};
}  // namespace

namespace cppkit {

void ErrorHandling::pushHandler(int channel,
                                ErrorHandling::ErrorHandler handler) {
  assert(channel >= 0);
  assert(channel < CHAN_COUNT_);
  handlers_[channel].push_back(handler);
}

ErrorHandling::ErrorHandler ErrorHandling::popHandler(int channel) {
  assert(channel >= 0);
  assert(channel < CHAN_COUNT_);
  assert(handlers_[channel].size() > 1);
  ErrorHandler handler = handlers_[channel].back();
  handlers_[channel].pop_back();
  return handler;
}

void ErrorHandling::resetHandler(int channel,
                                 ErrorHandling::ErrorHandler handler) {
  static const ErrorHandler default_handlers[CHAN_COUNT_] = {
      handle_assert_error, handle_check_error, handle_require_error,
      handle_ensure_error};
  assert(channel >= 0);
  assert(channel < CHAN_COUNT_);
  if (!handler) handler = default_handlers[channel];
  handlers_[channel].clear();
  handlers_[channel].push_back(handler);
}

void ErrorHandling::handleError_(int channel, const std::string &file, int line,
                                 const std::string &raw_expr,
                                 const std::string &eval_expr) {
  assert(channel >= 0);
  assert(channel < CHAN_COUNT_);
  handlers_[channel].back()(file, line, raw_expr, eval_expr);
}

}  // namespace cppkit

#endif
