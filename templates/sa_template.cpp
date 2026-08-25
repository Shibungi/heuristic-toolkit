#include <algorithm>
#include <iostream>
#include <numeric>
#include <vector>
#include "../library/search/simulated_annealing.hpp"
#include "../library/timer/timer.hpp"

struct Move { int i, j; };
struct State {
  std::vector<int> order;
  double value = 0;
  double score() const { return value; }
};

double contribution(int position, int value) { return -std::abs(position - value); }

int main() {
  int n; std::cin >> n;
  State state; state.order.resize(n); std::iota(state.order.begin(), state.order.end(), 0);
  ht::Timer timer(1.9, 0.03);
  ht::Annealer sa({100.0, 0.1, ht::CoolingSchedule::Exponential, ht::Objective::Maximize}, 1);
  State best = state; double best_score = state.score(); std::uint64_t iter = 0;
  while (!timer.poll(iter++)) {
    Move m{sa.rng().uniform_int(0, n - 1), sa.rng().uniform_int(0, n - 1)};
    if (m.i == m.j) continue;
    const double delta = contribution(m.i, state.order[m.j]) + contribution(m.j, state.order[m.i])
                       - contribution(m.i, state.order[m.i]) - contribution(m.j, state.order[m.j]);
    if (!sa.accept(delta, timer.progress())) continue;
    std::swap(state.order[m.i], state.order[m.j]); state.value += delta;
    if (state.value > best_score) best_score = state.value, best = state;
  }
  for (int x : best.order) std::cout << x << ' ';
  std::cout << '\n';
}
