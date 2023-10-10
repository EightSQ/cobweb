
#include "cobweb/core.hpp"
#include "cobweb/advanced.hpp"

namespace {

void advanced() {
  cw::BitCompressionTimerQueue<uint64_t, 4, 2> tq;
  tq.insert(1337, 2);
  tq.insert(1337, 4);

  assert(!tq.poll().has_value());
  assert(!tq.poll(1).has_value());
  assert(tq.poll(2).has_value());
  assert(!tq.poll(3).has_value());
}

void base() {
  cw::TimerQueue<uint64_t, 4> tq;
  tq.insert(1337, 2);
  tq.insert(1337, 4);

  assert(!tq.poll().has_value());
  assert(!tq.poll(1).has_value());
  assert(tq.poll(2).has_value());
  assert(!tq.poll(3).has_value());
}
}

int main() {
  base();

  advanced();

  return 0;
}
