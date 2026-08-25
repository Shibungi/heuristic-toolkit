#include <algorithm>
#include <cassert>
#include <cmath>
#include <iostream>
#include <numeric>
#include <vector>
#include "library/search/simulated_annealing.hpp"
#include "library/timer/timer.hpp"

// Complete delta-score example: maximize -sum(abs(position - value)).
// Input: n followed by an initial permutation of 0..n-1.
int main() {
  int n; std::cin >> n; std::vector<int> p(n); for (int& x : p) std::cin >> x;
  auto contribution = [](int position, int value) { return -std::abs(position - value); };
  auto full_score = [&] { long long s = 0; for (int i = 0; i < n; ++i) s += contribution(i, p[i]); return s; };
  long long score = full_score(), best_score = score; std::vector<int> best = p;
  ht::Timer timer(0.2, 0.005);
  ht::Annealer sa({20.0, 0.01, ht::CoolingSchedule::Exponential, ht::Objective::Maximize}, 12345);
  for (std::uint64_t iter = 0; !timer.poll(iter); ++iter) {
    int i = sa.rng().uniform_int(0, n - 1), j = sa.rng().uniform_int(0, n - 1);
    if (i == j) continue;
    long long delta = contribution(i, p[j]) + contribution(j, p[i])
                    - contribution(i, p[i]) - contribution(j, p[j]);
    if (!sa.accept(delta, timer.progress())) continue;
    std::swap(p[i], p[j]); score += delta;
#ifndef NDEBUG
    if ((iter & 4095) == 0) assert(score == full_score());
#endif
    if (score > best_score) best_score = score, best = p;
  }
  for (int x : best) std::cout << x << ' ';
  std::cout << "\n"; std::cerr << "Score = " << best_score << '\n';
}
