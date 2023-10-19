#pragma once

#include "cobweb/core.hpp"

namespace {}

namespace cw {

template <typename T, int k>
class BitCompressionTimerQueue : public MSBCompressionTimerQueue<std::tuple<Tick_t, int8_t, T>, k> {
  using ElementT =
      std::tuple<Tick_t, int8_t, T>;  // end time, bits_left, value
  using BaseTQ = MSBCompressionTimerQueue<ElementT, k>;

 public:
  explicit BitCompressionTimerQueue(int8_t num_max_bits) : num_max_bits_{num_max_bits} {}

  void insert(T value, Tick_t ttl) {
    this->insert(value, ttl, this->current_tick_);
  }
  void insert(T value, Tick_t ttl, Tick_t now) {
    BaseTQ::insert({now + ttl, num_max_bits_ - 1, value}, ttl, now);
  }

  std::optional<T> poll() { return this->poll(this->current_tick_); }
  std::optional<T> poll(Tick_t now) {
    std::optional<ElementT> el;
    while ((el = BaseTQ::poll(now)).has_value())
    {
      const auto& [due_time, bits_left, value] = el.value();
      if (due_time > this->current_tick_ && bits_left > 0) {
        BaseTQ::insert({due_time, bits_left - 1, value}, due_time - this->current_tick_, this->current_tick_);
      } else {
        return std::make_optional(value);
      }
    }
    return std::optional<T>();
  }
 private:
  const int8_t num_max_bits_;
};

}  // namespace cw
