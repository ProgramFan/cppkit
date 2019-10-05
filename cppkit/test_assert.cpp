#include <algorithm>
#include <cppkit/assert.hpp>
#include <iostream>
#include <vector>

void do_test() {
  int a = 2;
  CPPKIT_REQUIRE_GT(a, 1);
  std::vector<int> b{1, 2, 3};
  auto iter = std::find(b.begin(), b.end(), 1);
  std::unique_ptr<int> p;
  CPPKIT_ASSERT_TRUE(p);
  CPPKIT_ASSERT_NE(iter, b.end());
  CPPKIT_ASSERT_NE(&b[0], nullptr);
}

int main(int argc, char* argv[]) {
  do_test();
  return 0;
}
