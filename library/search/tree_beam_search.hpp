#pragma once

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <unordered_map>
#include <utility>
#include <vector>

namespace ht {

struct TreeBeamConfig {
  std::size_t width = 1000;
  bool deduplicate = true;
  bool maximize = true;
};

// Euler-tour (tree-based) beam search.
//
// Instead of storing one State per beam element, this stores the subtree spanned
// by their action histories. One mutable State is moved around the leaves with
// apply(state, action) / undo(state, action), avoiding State copies.
//
// expand(state, emit) must call:
//   emit(action, child_score, child_hash)
// for every child. expand must leave state unchanged. Action should be small,
// default-constructible, and exactly reversible by undo.
template <class State, class Action, class Score = double,
          class Hash = std::uint64_t>
class TreeBeamSearch {
 public:
  struct Result {
    std::vector<Action> actions;
    Score score{};
    Hash hash{};
    int depth = 0;
    std::uint64_t generated = 0;
    std::uint64_t retained = 0;
  };

 private:
  struct Event {
    Action action{};
    int leaf = -1;
    bool down = false;

    static Event down_edge(const Action& action) {
      Event event; event.action = action; event.down = true; return event;
    }
    static Event up_edge(int leaf = -1) {
      Event event; event.leaf = leaf; return event;
    }
  };

  struct Candidate {
    Action action;
    Score score;
    Hash hash;
    int parent_leaf;
  };

  TreeBeamConfig config_;
  std::vector<Event> tour_{Event::up_edge(0)};
  std::vector<Event> next_tour_;
  std::vector<Score> leaf_scores_;
  std::vector<Score> next_leaf_scores_;
  std::vector<Hash> leaf_hashes_;
  std::vector<Hash> next_leaf_hashes_;
  std::vector<Candidate> candidates_;
  std::vector<Action> path_workspace_;
  std::vector<int> head_, next_;

  bool better(const Score& lhs, const Score& rhs) const {
    return config_.maximize ? lhs > rhs : lhs < rhs;
  }

  template <class Expand, class Apply, class Undo>
  void generate_candidates(State& state, Expand& expand, Apply& apply, Undo& undo,
                           std::uint64_t& generated) {
    candidates_.clear();
    path_workspace_.clear();
    path_workspace_.reserve(tour_.size() / 2 + 1);
    for (const Event& event : tour_) {
      if (event.down) {
        apply(state, event.action);
        path_workspace_.push_back(event.action);
        continue;
      }
      if (event.leaf >= 0) {
        const int parent_leaf = event.leaf;
        expand(state, [&](Action action, Score score, Hash hash) {
          candidates_.push_back(
              Candidate{std::move(action), std::move(score), std::move(hash), parent_leaf});
          ++generated;
        });
      }
      if (path_workspace_.empty()) continue;  // final root sentinel
      undo(state, path_workspace_.back());
      path_workspace_.pop_back();
    }
    assert(path_workspace_.empty());
  }

  void deduplicate_candidates() {
    if (!config_.deduplicate) return;
    std::unordered_map<Hash, std::size_t> position;
    position.reserve(candidates_.size() * 2 + 1);
    std::vector<Candidate> unique;
    unique.reserve(candidates_.size());
    for (auto& candidate : candidates_) {
      auto [it, inserted] = position.emplace(candidate.hash, unique.size());
      if (inserted) {
        unique.push_back(std::move(candidate));
      } else if (better(candidate.score, unique[it->second].score)) {
        unique[it->second] = std::move(candidate);
      }
    }
    candidates_.swap(unique);
  }

  void select_candidates() {
    auto best_first = [&](const Candidate& lhs, const Candidate& rhs) {
      return better(lhs.score, rhs.score);
    };
    if (candidates_.size() > config_.width) {
      std::nth_element(candidates_.begin(),
                       candidates_.begin() + static_cast<std::ptrdiff_t>(config_.width),
                       candidates_.end(), best_first);
      candidates_.resize(config_.width);
    }
  }

  // Build the Euler tour of the minimal subtree spanning selected children.
  // Paths to parents with no retained child disappear without copying State.
  void rebuild_tour() {
    head_.assign(leaf_scores_.size(), -1);
    next_.assign(candidates_.size(), -1);
    for (int i = 0; i < static_cast<int>(candidates_.size()); ++i) {
      assert(0 <= candidates_[i].parent_leaf &&
             candidates_[i].parent_leaf < static_cast<int>(head_.size()));
      next_[i] = head_[candidates_[i].parent_leaf];
      head_[candidates_[i].parent_leaf] = i;
    }

    next_tour_.clear();
    next_tour_.reserve(tour_.size() + 2 * candidates_.size() + 1);
    next_leaf_scores_.clear();
    next_leaf_hashes_.clear();
    next_leaf_scores_.reserve(candidates_.size());
    next_leaf_hashes_.reserve(candidates_.size());
    path_workspace_.clear();
    path_workspace_.reserve(tour_.size() / 2 + 1);
    std::size_t committed = 0;
    for (const Event& event : tour_) {
      if (event.down) {
        path_workspace_.push_back(event.action);
        continue;
      }

      if (event.leaf >= 0 && head_[event.leaf] != -1) {
        while (committed < path_workspace_.size()) {
          next_tour_.push_back(Event::down_edge(path_workspace_[committed]));
          ++committed;
        }
        for (int index = head_[event.leaf]; index != -1; index = next_[index]) {
          const int new_leaf = static_cast<int>(next_leaf_scores_.size());
          next_tour_.push_back(Event::down_edge(candidates_[index].action));
          next_tour_.push_back(Event::up_edge(new_leaf));
          next_leaf_scores_.push_back(candidates_[index].score);
          next_leaf_hashes_.push_back(candidates_[index].hash);
        }
      }

      if (path_workspace_.empty()) {
        next_tour_.push_back(Event::up_edge());  // root sentinel
        break;
      }
      if (committed == path_workspace_.size()) {
        next_tour_.push_back(Event::up_edge());
        --committed;
      }
      path_workspace_.pop_back();
    }

    assert(next_leaf_scores_.size() == candidates_.size());
    tour_.swap(next_tour_);
    leaf_scores_.swap(next_leaf_scores_);
    leaf_hashes_.swap(next_leaf_hashes_);
  }

  std::vector<Action> path_to_leaf(int target_leaf) const {
    std::vector<Action> path;
    path.reserve(tour_.size() / 2 + 1);
    for (const Event& event : tour_) {
      if (event.down) {
        path.push_back(event.action);
        continue;
      }
      if (event.leaf == target_leaf) return path;
      if (!path.empty()) path.pop_back();
    }
    return {};
  }

  Result current_best(int depth, std::uint64_t generated,
                      std::uint64_t retained) const {
    assert(!leaf_scores_.empty());
    int best = 0;
    for (int i = 1; i < static_cast<int>(leaf_scores_.size()); ++i)
      if (better(leaf_scores_[i], leaf_scores_[best])) best = i;
    return Result{path_to_leaf(best), leaf_scores_[best], leaf_hashes_[best],
                  depth, generated, retained};
  }

 public:
  explicit TreeBeamSearch(TreeBeamConfig config = {}) : config_(config) {
    assert(config_.width > 0);
  }

  template <class Expand, class Apply, class Undo>
  Result solve(State initial, Score initial_score, Hash initial_hash,
               int max_depth, Expand expand, Apply apply, Undo undo) {
    tour_.assign(1, Event::up_edge(0));
    leaf_scores_.assign(1, initial_score);
    leaf_hashes_.assign(1, initial_hash);
    candidates_.clear();
    std::uint64_t generated = 0, retained = 1;
    int reached_depth = 0;

    for (int depth = 0; depth < max_depth; ++depth) {
      generate_candidates(initial, expand, apply, undo, generated);
      if (candidates_.empty()) break;
      deduplicate_candidates();
      select_candidates();
      retained += candidates_.size();
      rebuild_tour();
      reached_depth = depth + 1;
    }
    return current_best(reached_depth, generated, retained);
  }
};

}  // namespace ht
