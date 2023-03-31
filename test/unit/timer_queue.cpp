#include <utility>

#include "cobweb/cobweb.hpp"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

using ::testing::Optional;

namespace cw::test {

class TimerQueueTest : public ::testing::Test {};

TEST_F(TimerQueueTest, Initialization) { TimerQueue<uint64_t, 16> tq; }

TEST_F(TimerQueueTest, Initialization2) { TimerQueue<char[128], 16> tq; }

TEST_F(TimerQueueTest, Basic) {
  TimerQueue<uint64_t, 16> tq;

  EXPECT_TRUE(tq.empty());

  tq.insert(1337, 2);
  tq.insert(1338, 4);

  EXPECT_FALSE(tq.empty());

  EXPECT_FALSE(tq.poll().has_value());
  EXPECT_FALSE(tq.poll(1).has_value());

  EXPECT_THAT(tq.poll(2), Optional(1337));
  EXPECT_EQ(tq.poll(), std::nullopt);
  EXPECT_EQ(tq.poll(2), std::nullopt);
  EXPECT_EQ(tq.poll(3), std::nullopt);
  EXPECT_EQ(tq.poll(), std::nullopt);
  tq.insert(1339, 2, 3);
  EXPECT_THAT(tq.poll(4), Optional(1338));
  EXPECT_EQ(tq.poll(), std::nullopt);
  EXPECT_EQ(tq.poll(4), std::nullopt);
  EXPECT_THAT(tq.poll(5), Optional(1339));

  EXPECT_TRUE(tq.empty());
}

TEST_F(TimerQueueTest, TTLPruning) {
  TimerQueue<uint64_t, 16> tq;
  tq.insert(1337, 11, 0);

  EXPECT_EQ(tq.next_timeout(), 8);

  EXPECT_EQ(tq.poll(7), std::nullopt);
  EXPECT_THAT(tq.poll(8), Optional(1337));
  EXPECT_EQ(tq.poll(10), std::nullopt);
  EXPECT_TRUE(tq.empty());
}

} // namespace cw::test
