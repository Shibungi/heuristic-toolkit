#pragma once

#include <cstdint>
#include <utility>
#include "../random/rng.hpp"

namespace ht {

template <class State, class Move, class Propose, class Delta, class Apply>
State hill_climb(State initial, std::uint64_t iterations, std::uint64_t seed,
                 Propose propose, Delta delta_score, Apply apply) {
  Xoshiro256StarStar rng(seed);
  State current = std::move(initial), best = current;
  double score = current.score(), best_score = score;
  for (std::uint64_t i = 0; i < iterations; ++i) {
    Move move = propose(current, rng);
    const double delta = delta_score(current, move);
    if (delta < 0) continue;
    apply(current, move); score += delta;
    if (score > best_score) best_score = score, best = current;
  }
  return best;
}

}  // namespace ht
