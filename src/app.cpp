
#include "cobweb/cobweb.hpp"

int main() {
  cw::TimerQueue<uint64_t, 4> tq;
  tq.insert(1337, 2);
  tq.insert(1337, 4);

  assert(!tq.poll().has_value());
  assert(!tq.poll(1).has_value());
  assert(tq.poll(2).has_value());
  assert(!tq.poll(3).has_value());

  return 0;
}
