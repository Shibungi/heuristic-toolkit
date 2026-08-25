#pragma once

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <numeric>
#include <vector>
#include "../random/rng.hpp"

namespace ht {

enum class Objective { Maximize, Minimize };
enum class CoolingSchedule { Linear, Exponential };

struct SAConfig {
  double start_temperature = 100.0;
  double end_temperature = 0.1;
  CoolingSchedule schedule = CoolingSchedule::Exponential;
  Objective objective = Objective::Maximize;
};

class Annealer {
  SAConfig config_;
  Xoshiro256StarStar rng_;
  double heat_ = 1.0;

 public:
  explicit Annealer(SAConfig config = {}, std::uint64_t seed = 1)
      : config_(config), rng_(seed) {}
  double temperature(double progress) const {
    progress = std::clamp(progress, 0.0, 1.0);
    if (config_.schedule == CoolingSchedule::Linear)
      return heat_ * std::lerp(config_.start_temperature, config_.end_temperature, progress);
    if (config_.start_temperature <= 0 || config_.end_temperature <= 0) return 0;
    return heat_ * config_.start_temperature *
           std::pow(config_.end_temperature / config_.start_temperature, progress);
  }
  bool accept(double delta, double progress) {
    const double gain = config_.objective == Objective::Maximize ? delta : -delta;
    if (gain >= 0) return true;
    const double t = temperature(progress);
    return t > 0 && rng_.uniform01() < std::exp(gain / t);
  }
  bool better(double lhs, double rhs) const {
    return config_.objective == Objective::Maximize ? lhs > rhs : lhs < rhs;
  }
  void reheat(double factor = 2.0) { heat_ *= factor; }
  void reset_temperature() { heat_ = 1.0; }
  Xoshiro256StarStar& rng() { return rng_; }
};

// Lightweight adaptive neighborhood selector. Reward accepted/improving moves;
// call update_epoch periodically so probabilities do not oscillate every move.
class AdaptiveNeighborhoods {
  std::vector<double> weights_, reward_;
  std::vector<std::uint64_t> trials_;
  double floor_;

 public:
  explicit AdaptiveNeighborhoods(int count, double floor_weight = 0.05)
      : weights_(count, 1.0), reward_(count), trials_(count), floor_(floor_weight) {}
  int choose(Xoshiro256StarStar& rng) const {
    const double sum = std::accumulate(weights_.begin(), weights_.end(), 0.0);
    double x = rng.uniform01() * sum;
    for (int i = 0; i < static_cast<int>(weights_.size()); ++i)
      if ((x -= weights_[i]) <= 0) return i;
    return static_cast<int>(weights_.size()) - 1;
  }
  void observe(int id, bool accepted, bool improved) {
    ++trials_[id]; reward_[id] += improved ? 4.0 : accepted ? 1.0 : 0.0;
  }
  void update_epoch(double learning_rate = 0.2) {
    for (int i = 0; i < static_cast<int>(weights_.size()); ++i) {
      const double quality = trials_[i] ? reward_[i] / trials_[i] : 0.0;
      weights_[i] = std::max(floor_, (1.0 - learning_rate) * weights_[i] + learning_rate * quality);
      reward_[i] = 0; trials_[i] = 0;
    }
  }
  const std::vector<double>& weights() const { return weights_; }
};

template <class State, class Move, class Propose, class Delta, class Apply, class Progress>
State run_delta_sa(State initial, std::uint64_t iterations, Annealer& sa,
                   Propose propose, Delta delta_score, Apply apply, Progress progress) {
  State current = std::move(initial), best = current;
  double score = current.score(), best_score = score;
  for (std::uint64_t iter = 0; iter < iterations; ++iter) {
    Move move = propose(current, sa.rng());
    const double delta = delta_score(current, move);
    if (!sa.accept(delta, progress(iter, iterations))) continue;
    apply(current, move); score += delta;
    const bool improved = sa.better(score, best_score);
    if (improved) best_score = score, best = current;  // copy only on improvement
  }
  return best;
}

// For moves whose exact delta is easiest to know after mutation.
// apply returns delta; rollback must exactly undo apply when rejected.
template <class State, class Move, class Propose, class Apply, class Rollback, class Progress>
State run_transactional_sa(State initial, std::uint64_t iterations, Annealer& sa,
                           Propose propose, Apply apply, Rollback rollback, Progress progress) {
  State current = std::move(initial), best = current;
  double score = current.score(), best_score = score;
  for (std::uint64_t iter = 0; iter < iterations; ++iter) {
    Move move = propose(current, sa.rng());
    const double delta = apply(current, move);
    if (!sa.accept(delta, progress(iter, iterations))) { rollback(current, move); continue; }
    score += delta;
    if (sa.better(score, best_score)) best_score = score, best = current;
  }
  return best;
}

}  // namespace ht
