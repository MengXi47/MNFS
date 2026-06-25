#include <folly/logging/xlog.h>
#include <iostream>

int main() {
  std::cout << "Hello, World!" << std::endl;
  XLOG(INFO, "Hello, World!");
  return 0;
}