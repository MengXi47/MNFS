#include <folly/logging/xlog.h>

#include "common/utils/Result.h"

int main(int argc, char *argv[]) {
  using namespace mnfs;
  XLOG(INFO, "Hello World");
  return 0;
}