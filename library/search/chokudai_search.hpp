#pragma once

#include <cstddef>
#include <queue>
#include <utility>
#include <vector>

namespace ht {

// Node::operator< should rank a better node higher for priority_queue.
// Repeated sweeps preserve promising nodes at every depth unlike layered beam.
template <class Node, class Expand>
Node chokudai_search(Node initial, int max_depth, int sweeps, std::size_t width, Expand expand) {
  std::vector<std::priority_queue<Node>> queues(max_depth + 1);
  queues[0].push(std::move(initial));
  for (int sweep = 0; sweep < sweeps; ++sweep) {
    for (int d = 0; d < max_depth; ++d) {
      std::vector<Node> selected;
      for (std::size_t k = 0; k < width && !queues[d].empty(); ++k) {
        selected.push_back(queues[d].top()); queues[d].pop();
      }
      for (const auto& node : selected)
        expand(node, [&](Node child) { queues[d + 1].push(std::move(child)); });
    }
  }
  return queues[max_depth].top();
}

}  // namespace ht
