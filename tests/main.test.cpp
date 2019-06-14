#define DOCTEST_CONFIG_IMPLEMENT
#include <doctest/doctest.h>
#include <dlog.hpp>
#include "main.test.hpp"

int main(int argc, char** argv) {
  DLOG_INIT();
  //DLOG_SET_LEVEL(dlog::Level::kError);

  doctest::Context context;
  const int res = context.run();

  // if(context.shouldExit()) {
  //       return res;
  // }

  return res;
}
