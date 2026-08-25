#pragma once

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <unordered_map>
#include <utility>
#include <vector>

namespace ht {

struct BeamConfig {
  std::size_t width = 100;
  bool deduplicate = true;
  bool maximize = true;
};

// expand(node, emit) calls emit(child) for each candidate. Node must expose
// .score and .hash. This deliberately keeps State/Action/Parent policy in Node.
template <class Node, class Expand>
Node layered_beam_search(Node initial, int depth, BeamConfig config, Expand expand) {
  std::vector<Node> current{std::move(initial)}, next;
  auto better = [&](const Node& a, const Node& b) {
    return config.maximize ? a.score > b.score : a.score < b.score;
  };
  for (int d = 0; d < depth && !current.empty(); ++d) {
    next.clear();
    for (const auto& node : current) expand(node, [&](Node child) { next.push_back(std::move(child)); });
    if (config.deduplicate) {
      std::unordered_map<std::uint64_t, std::size_t> at;
      std::vector<Node> unique; unique.reserve(next.size());
      for (auto& node : next) {
        auto [it, inserted] = at.emplace(node.hash, unique.size());
        if (inserted) unique.push_back(std::move(node));
        else if (better(node, unique[it->second])) unique[it->second] = std::move(node);
      }
      next.swap(unique);
    }
    if (next.size() > config.width) {
      std::nth_element(next.begin(), next.begin() + config.width, next.end(), better);
      next.resize(config.width);
    }
    current.swap(next);
  }
  return *std::max_element(current.begin(), current.end(),
                           [&](const Node& a, const Node& b) { return better(b, a); });
}

}  // namespace ht
