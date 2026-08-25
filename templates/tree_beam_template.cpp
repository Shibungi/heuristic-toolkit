#include <array>
#include <cstdint>
#include <iostream>
#include "../library/search/tree_beam_search.hpp"

// Copy-free beam example. Replace State/Action and the four lambdas.
struct Action { int value = 0; };
struct State {
  int depth = 0;
  int sum = 0;
  std::array<int, 1024> large_workspace{};  // never copied per beam element
};

int main() {
  int max_depth; std::cin >> max_depth;
  State initial;
  ht::TreeBeamSearch<State, Action, int> beam({.width = 1000});
  auto result = beam.solve(
      initial, 0, 0, max_depth,
      [](State& state, auto emit) {
        for (int value = 0; value < 3; ++value) {
          const int child_score = state.sum + value;
          const std::uint64_t child_hash =
              static_cast<std::uint64_t>(state.depth + 1) * 1000003 + child_score;
          emit(Action{value}, child_score, child_hash);
        }
      },
      [](State& state, const Action& action) {
        ++state.depth; state.sum += action.value;
      },
      [](State& state, const Action& action) {
        state.sum -= action.value; --state.depth;
      });

  for (const Action& action : result.actions) std::cout << action.value << '\n';
  std::cerr << "score=" << result.score << " generated=" << result.generated
            << " retained=" << result.retained << '\n';
}
