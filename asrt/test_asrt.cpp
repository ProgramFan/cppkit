#include <iostream>

#include "asrt.h"

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
  asrt::ErrorHandling::pushHandler(asrt::CHAN_ASSERT, MyAssertHandler);
  ASSERT_TRUE(f1());
  ASSERT_FALSE(f2());
  ASSERT_EQ(f1_count, 1);
  ASSERT_EQ(f2_count, 1);
  ASSERT_EQ(f1(), f2());
  asrt::ErrorHandling::popHandler(asrt::CHAN_ASSERT);
  int x = 1, y = 2;
  ASSERT_EQ(x, y);
  ASSERT_NE(x, y);
  ASSERT_GT(x, y);
  ASSERT_GE(x, y);
  ASSERT_LT(x, y);
  ASSERT_LE(x, y);
}

int main(int argc, char *argv[]) {
  do_test();
  return 0;
}
