#pragma once

#include <algorithm>
#include <array>
#include <cassert>
#include <cstdint>
#include <limits>
#include <optional>
#include <queue>
#include <utility>

namespace cw {

using Tick_t = uint64_t;
constexpr Tick_t MAX_TICK = std::numeric_limits<Tick_t>::max();

/// TimerQueue with k possible TTLs (1, 2, 4, ..., 2^(k-1)).
template <typename T, int k> class TimerQueue {
  using QueueElement_t = std::pair<Tick_t, T>;

public:
  void insert(T value, Tick_t ttl) { insert(value, ttl, current_tick_); }
  void insert(T value, Tick_t ttl, Tick_t now) {
    assert(current_tick_ <= now);
    current_tick_ = now;

    assert(ttl > 0);
    // Use highest bit as TTL.
    int queue_index = 63 - __builtin_clzll(ttl);
    assert(queue_index < k);

    queues_[queue_index].emplace(current_tick_ + (1ull << queue_index), value);
    ++num_timers_scheduled_;
  }

  std::optional<T> poll() { return poll(current_tick_); }
  std::optional<T> poll(Tick_t now) {
    assert(current_tick_ <= now);
    current_tick_ = now;

    // no active timers -> no element due
    if (empty()) {
      return std::optional<T>();
    }

    auto [next_deadline, queue_index] = find_next_due_queue();

    // element with smallest deadline is not due
    if (next_deadline > current_tick_) {
      return std::optional<T>();
    }
    assert(queue_index >= 0);
    assert(queue_index < k);

    // remove due element from queue
    std::queue<QueueElement_t> &target_queue = queues_[queue_index];
    std::optional<T> result = std::make_optional(target_queue.front().second);
    target_queue.pop();

    // and return it
    --num_timers_scheduled_;
    return result;
  }

  bool empty() const { return num_timers_scheduled_ == 0; }

  Tick_t next_timeout() const {
    auto [deadline, _] = find_next_due_queue();
    return deadline;
  }

private:
  /// Find smallest deadline among front elements.
  std::pair<Tick_t, int> find_next_due_queue() const {
    std::pair<Tick_t, int> earliest_element_due = {MAX_TICK, -1};
    for (int queue_index = 0; queue_index < k; ++queue_index) {
      const std::queue<QueueElement_t> &q = queues_[queue_index];
      if (!q.empty()) {
        earliest_element_due =
            min(earliest_element_due, {q.front().first, queue_index});
      }
    }
    return earliest_element_due;
  }

  std::array<std::queue<QueueElement_t>, k> queues_{};
  Tick_t current_tick_{0};
  uint64_t num_timers_scheduled_{0};
};

} // namespace cw
