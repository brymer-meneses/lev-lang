#include "lev.h"

auto main(const int argc, const char** argv) -> int {

  lev::Lev lev(argv);
  lev.compile();

  return 0;
}
