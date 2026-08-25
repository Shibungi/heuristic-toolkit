#pragma once

#include <array>
#include <cassert>
#include <cstdint>
#include <limits>

namespace ht {

class SplitMix64 {
  std::uint64_t x_;

 public:
  explicit SplitMix64(std::uint64_t seed = 0) : x_(seed) {}
  std::uint64_t operator()() {
    std::uint64_t z = (x_ += 0x9e3779b97f4a7c15ULL);
    z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9ULL;
    z = (z ^ (z >> 27)) * 0x94d049bb133111ebULL;
    return z ^ (z >> 31);
  }
};

class Xoshiro256StarStar {
  std::array<std::uint64_t, 4> s_{};
  static constexpr std::uint64_t rotl(std::uint64_t x, int k) {
    return (x << k) | (x >> (64 - k));
  }

 public:
  using result_type = std::uint64_t;
  explicit Xoshiro256StarStar(std::uint64_t seed = 1) { reseed(seed); }
  void reseed(std::uint64_t seed) {
    SplitMix64 sm(seed);
    for (auto& x : s_) x = sm();
  }
  static constexpr result_type min() { return 0; }
  static constexpr result_type max() { return std::numeric_limits<result_type>::max(); }
  std::uint64_t operator()() {
    const std::uint64_t result = rotl(s_[1] * 5, 7) * 9;
    const std::uint64_t t = s_[1] << 17;
    s_[2] ^= s_[0]; s_[3] ^= s_[1]; s_[1] ^= s_[2]; s_[0] ^= s_[3];
    s_[2] ^= t; s_[3] = rotl(s_[3], 45);
    return result;
  }
  std::uint64_t uniform_u64(std::uint64_t bound) {
    assert(bound > 0);
    const std::uint64_t threshold = -bound % bound;
    while (true) {
      const auto x = (*this)();
      if (x >= threshold) return x % bound;
    }
  }
  int uniform_int(int lo, int hi) {
    assert(lo <= hi);
    return lo + static_cast<int>(uniform_u64(static_cast<std::uint64_t>(hi) - lo + 1));
  }
  double uniform01() { return ((*this)() >> 11) * 0x1.0p-53; }
  bool bernoulli(double p) { return uniform01() < p; }
};

}  // namespace ht
