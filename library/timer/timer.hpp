#pragma once

#include <algorithm>
#include <chrono>
#include <cstdint>

namespace ht {

class Timer {
  using Clock = std::chrono::steady_clock;
  Clock::time_point start_ = Clock::now();
  double limit_sec_ = 0.0;
  double safety_sec_ = 0.0;
  mutable double cached_sec_ = 0.0;

 public:
  explicit Timer(double limit_sec = 0.0, double safety_sec = 0.0)
      : limit_sec_(limit_sec), safety_sec_(safety_sec) {}
  void reset() { start_ = Clock::now(); cached_sec_ = 0.0; }
  double update() const {
    cached_sec_ = std::chrono::duration<double>(Clock::now() - start_).count();
    return cached_sec_;
  }
  double elapsed() const { return cached_sec_; }
  double elapsed_exact() const { return update(); }
  bool expired() const { return update() >= std::max(0.0, limit_sec_ - safety_sec_); }
  bool expired_cached() const { return cached_sec_ >= std::max(0.0, limit_sec_ - safety_sec_); }
  double progress() const {
    const double usable = std::max(1e-12, limit_sec_ - safety_sec_);
    return std::clamp(cached_sec_ / usable, 0.0, 1.0);
  }
  // Use inside a hot loop: if (timer.poll(iter, 255)) break;
  bool poll(std::uint64_t iter, std::uint64_t mask = 255) const {
    return ((iter & mask) == 0) ? expired() : expired_cached();
  }
};

}  // namespace ht
