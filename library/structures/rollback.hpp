#pragma once

#include <cstddef>
#include <utility>
#include <vector>

namespace ht {

template <class T>
class UndoLog {
  struct Entry { T* address; T old_value; };
  std::vector<Entry> log_;

 public:
  using Checkpoint = std::size_t;
  explicit UndoLog(std::size_t reserve = 0) { log_.reserve(reserve); }
  Checkpoint checkpoint() const { return log_.size(); }
  void assign(T& target, T value) {
    log_.push_back({&target, target}); target = std::move(value);
  }
  void rollback(Checkpoint cp) {
    while (log_.size() > cp) {
      auto& e = log_.back(); *e.address = std::move(e.old_value); log_.pop_back();
    }
  }
  void commit(Checkpoint cp = 0) {
    if (cp == 0) log_.clear();
    else log_.erase(log_.begin() + static_cast<std::ptrdiff_t>(cp), log_.end());
  }
};

}  // namespace ht
