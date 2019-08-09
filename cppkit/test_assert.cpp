#include <iostream>

#include "assert.hpp"

namespace {
void MyAssertHandler(const std::string &file, int line,
                     const std::string &raw_expr,
                     const std::string &eval_expr) {
  std::cout << file << ":" << line << ": assert(" << raw_expr
            << ") failed, values (" << eval_expr << ")" << std::endl;
}
}  // namespace

// Check if the arguments are evaluated more than once.
int f1_count = 0;
int f2_count = 0;
int f1() {
  f1_count++;
  return 1;
}
int f2() {
  f2_count++;
  return 2;
}

void do_test() {
  cppkit::ErrorHandling::pushHandler(cppkit::CHAN_ASSERT, MyAssertHandler);
  CPPKIT_ASSERT_TRUE(f1());
  CPPKIT_ASSERT_FALSE(f2());
  CPPKIT_ASSERT_EQ(f1_count, 1);
  CPPKIT_ASSERT_EQ(f2_count, 1);
  CPPKIT_ASSERT_EQ(f1(), f2());
  cppkit::ErrorHandling::popHandler(cppkit::CHAN_ASSERT);
  int x = 1, y = 2;
  CPPKIT_ASSERT_EQ(x, y);
  CPPKIT_ASSERT_NE(x, y);
  CPPKIT_ASSERT_GT(x, y);
  CPPKIT_ASSERT_GE(x, y);
  CPPKIT_ASSERT_LT(x, y);
  CPPKIT_ASSERT_LE(x, y);
}

int main(int argc, char *argv[]) {
  do_test();
  return 0;
}
