#include <iostream>
#include <vector>
#include "../library/search/mcts.hpp"

struct State { int depth = 0, value = 0; };
struct Model {
  std::vector<int> actions(const State& s) { return terminal(s) ? std::vector<int>{} : std::vector<int>{-1, 1}; }
  State next(State s, int action) { ++s.depth; s.value += action; return s; }
  bool terminal(const State& s) { return s.depth == 10; }
  double evaluate(const State& s) { return s.value; }
  double prior(const State&, int) { return 0.5; }
  int rollout_action(const State&, const std::vector<int>& actions, ht::Xoshiro256StarStar& rng) {
    return actions[rng.uniform_u64(actions.size())];
  }
};

int main() {
  Model model;
  std::cout << ht::mcts<State, int>(State{}, model, {.iterations = 5000, .rollout_depth = 10}, 1) << '\n';
}
