#include <algorithm>
#include <random>
#include <utility>
#include <vector>

#include "cobweb/core.hpp"
#include "cobweb/advanced.hpp"
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

TEST_F(TimerQueueTest, Advanced) {
  constexpr int K = 8;
  TimerQueue<uint64_t, K> tq;
  std::mt19937 gen(42);
  std::uniform_int_distribution<uint64_t> val_dist;
  std::uniform_int_distribution<uint64_t> start_time_test(1, (1u << 16));
  std::uniform_int_distribution<uint64_t> ttl_bit_dist(1, K - 1);
  std::vector<std::pair<std::pair<uint64_t, int>, uint64_t>>
      inputs; // ((starttime, ttl), value)
  std::vector<std::pair<std::pair<uint64_t, int>, uint64_t>>
      expected_outputs; // ((deadline, ttl), value)

  constexpr int num_objects = (1 << 16);
  for (int i = 0; i < num_objects; ++i) {
    uint64_t start_time = start_time_test(gen);
    uint64_t ttl = (1ull << ttl_bit_dist(gen));
    uint64_t deadline_time = start_time + ttl;
    uint64_t value = val_dist(gen);
    inputs.push_back({{start_time, ttl}, value});
    expected_outputs.push_back({{start_time + ttl, ttl}, value});
  }

  std::sort(inputs.begin(), inputs.end());
  std::sort(expected_outputs.begin(), expected_outputs.end());

  int input_index = 0;
  int output_index = 0;
  for (uint64_t ts = 0; ts <= expected_outputs.back().first.first; ++ts) {
    while (input_index < num_objects && inputs[input_index].first.first == ts) {
      const auto &next_input = inputs[input_index++];
      tq.insert(next_input.second, next_input.first.second, ts);
    }
    while (output_index < num_objects &&
           expected_outputs[output_index].first.first == ts) {
      const auto &next_output = expected_outputs[output_index++];
      EXPECT_THAT(tq.poll(ts), Optional(next_output.second));
    }
    EXPECT_EQ(tq.poll(ts), std::nullopt);
  }

  EXPECT_TRUE(tq.empty());
  EXPECT_EQ(tq.poll(), std::nullopt);
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

TEST_F(TimerQueueTest, BitCompressionTTLPruning) {
  BitCompressionTimerQueue<uint64_t, 16> tq{2};
  tq.insert(1337, 11, 0);

  EXPECT_EQ(tq.next_timeout(), 8);

  EXPECT_EQ(tq.poll(7), std::nullopt);
  EXPECT_EQ(tq.poll(8), std::nullopt);
  EXPECT_THAT(tq.poll(10), Optional(1337));
  EXPECT_TRUE(tq.empty());
}

} // namespace cw::test
