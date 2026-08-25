#include <cstdint>
#include <iostream>
#include <vector>
#include "../library/search/beam_search.hpp"

struct Node {
  std::vector<int> state;
  std::vector<int> actions;
  double score = 0;
  std::uint64_t hash = 0;
};

int main() {
  int depth; std::cin >> depth;
  Node answer = ht::layered_beam_search(Node{}, depth, {.width = 200},
    [](const Node& node, auto emit) {
      for (int action = 0; action < 3; ++action) {
        Node child = node;  // Replace with rollback/action-only storage if State is large.
        child.state.push_back(action); child.actions.push_back(action);
        child.score += action; child.hash = child.hash * 1000003 + action + 1;
        emit(std::move(child));
      }
    });
  for (int action : answer.actions) std::cout << action << '\n';
}
