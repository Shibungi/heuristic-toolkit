#pragma once

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <memory>
#include <unordered_map>
#include <utility>
#include <vector>
#include "../random/rng.hpp"

namespace ht {

struct MCTSConfig {
  int iterations = 1000;
  int rollout_depth = 100;
  double exploration = 1.41421356237;
  double puct = 0.0;                 // >0 selects PUCT, otherwise UCB1
  double progressive_widening_k = 0.0;  // 0 disables; allowed ~= k*sqrt(visits)
};

struct TranspositionStat { std::uint64_t visits = 0; double value_sum = 0; };
using TranspositionTable = std::unordered_map<std::uint64_t, TranspositionStat>;

// Model interface:
// actions(state)->vector<Action>, next(state, action)->State,
// terminal(state)->bool, evaluate(state)->double, prior(state, action)->double.
// State copies are explicit here: for expensive states, copy this file and replace
// next() with apply/rollback rather than hiding the cost behind an abstraction.
template <class State, class Action, class Model>
Action mcts(const State& root_state, Model& model, MCTSConfig cfg, std::uint64_t seed = 1) {
  struct Node {
    State state; Node* parent = nullptr; Action action{}; double prior = 1.0;
    std::uint64_t visits = 0; double value_sum = 0;
    std::vector<Action> untried; std::vector<std::unique_ptr<Node>> children;
    Node(State s, Node* p, Action a, double pr, Model& m)
        : state(std::move(s)), parent(p), action(std::move(a)), prior(pr), untried(m.actions(state)) {}
    double mean() const { return visits ? value_sum / visits : 0.0; }
  };
  Xoshiro256StarStar rng(seed);
  Node root(root_state, nullptr, Action{}, 1.0, model);
  for (int iteration = 0; iteration < cfg.iterations; ++iteration) {
    Node* node = &root;
    while (!model.terminal(node->state)) {
      std::size_t allowed = node->untried.size() + node->children.size();
      if (cfg.progressive_widening_k > 0)
        allowed = std::max<std::size_t>(1, static_cast<std::size_t>(cfg.progressive_widening_k * std::sqrt(node->visits + 1.0)));
      if (!node->untried.empty() && node->children.size() < allowed) {
        const std::size_t i = rng.uniform_u64(node->untried.size());
        Action a = node->untried[i]; node->untried[i] = node->untried.back(); node->untried.pop_back();
        State next = model.next(node->state, a);
        node->children.push_back(std::make_unique<Node>(std::move(next), node, a, model.prior(node->state, a), model));
        node = node->children.back().get(); break;
      }
      if (node->children.empty()) break;
      const double log_parent = std::log(static_cast<double>(node->visits) + 1.0);
      node = std::max_element(node->children.begin(), node->children.end(), [&](const auto& x, const auto& y) {
        auto value = [&](const auto& c) {
          if (!c->visits) return std::numeric_limits<double>::infinity();
          if (cfg.puct > 0) return c->mean() + cfg.puct * c->prior * std::sqrt(node->visits + 1.0) / (1 + c->visits);
          return c->mean() + cfg.exploration * std::sqrt(log_parent / c->visits);
        };
        return value(x) < value(y);
      })->get();
    }
    State rollout = node->state;
    for (int d = 0; d < cfg.rollout_depth && !model.terminal(rollout); ++d) {
      auto actions = model.actions(rollout); if (actions.empty()) break;
      rollout = model.next(rollout, model.rollout_action(rollout, actions, rng));
    }
    const double value = model.evaluate(rollout);
    for (; node; node = node->parent) { ++node->visits; node->value_sum += value; }
  }
  const auto best = std::max_element(root.children.begin(), root.children.end(),
                                     [](const auto& a, const auto& b) { return a->visits < b->visits; });
  return (*best)->action;
}

}  // namespace ht
