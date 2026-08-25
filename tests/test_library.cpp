#include <cassert>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <numeric>
#include <vector>
#include "library/random/rng.hpp"
#include "library/search/beam_search.hpp"
#include "library/search/chokudai_search.hpp"
#include "library/search/simulated_annealing.hpp"
#include "library/graph/dsu.hpp"
#include "library/structures/fast_clear_array.hpp"
#include "library/structures/fast_set.hpp"
#include "library/structures/rollback.hpp"
#include "library/structures/zobrist_hash.hpp"
#include "library/timer/timer.hpp"

void test_rng() {
  ht::Xoshiro256StarStar a(42), b(42);
  for (int i = 0; i < 10000; ++i) assert(a() == b());
  for (int bound = 1; bound < 100; ++bound) for (int i = 0; i < 100; ++i) assert(a.uniform_u64(bound) < static_cast<unsigned>(bound));
}
void test_structures() {
  ht::FastSet s(10); assert(s.insert(3)); assert(!s.insert(3)); assert(s.erase(3)); assert(!s.contains(3));
  ht::FastClearArray<int> a(4, -1); a.set(2, 7); assert(a.get(2) == 7); a.clear(); assert(a.get(2) == -1);
  int x = 1, y = 2; ht::UndoLog<int> log; auto cp = log.checkpoint(); log.assign(x, 5); log.assign(y, 8); log.rollback(cp); assert(x == 1 && y == 2);
  ht::ZobristTable z(3, 4, 1); auto h = z.at(1, 2); z.change(h, 1, 2, 3); assert(h == z.at(1, 3));
  ht::DSU dsu(4); assert(dsu.merge(0, 1)); assert(dsu.same(0, 1)); assert(dsu.size(0) == 2);
  ht::RollbackDSU rdsu(4); auto dcp = rdsu.checkpoint(); rdsu.merge(1, 2); assert(rdsu.same(1, 2)); rdsu.rollback(dcp); assert(!rdsu.same(1, 2));
}
void test_incremental_score() {
  ht::Xoshiro256StarStar rng(7); std::vector<int> p(40); std::iota(p.begin(), p.end(), 0);
  auto full = [&] { long long v = 0; for (int i = 0; i < 40; ++i) v -= std::abs(i - p[i]); return v; };
  long long score = full();
  for (int k = 0; k < 100000; ++k) {
    int i = rng.uniform_int(0, 39), j = rng.uniform_int(0, 39);
    long long before = -std::abs(i - p[i]) - std::abs(j - p[j]);
    long long after = -std::abs(i - p[j]) - std::abs(j - p[i]);
    std::swap(p[i], p[j]); score += after - before; assert(score == full());
  }
}
void test_beam() {
  struct N { int depth = 0; double score = 0; std::uint64_t hash = 0; };
  N got = ht::layered_beam_search(N{}, 5, {.width = 2}, [](const N& n, auto emit) {
    for (int x = 0; x < 2; ++x) emit(N{n.depth + 1, n.score + x, n.hash * 3 + x + 1});
  });
  assert(got.score == 5);
  N deduped = ht::layered_beam_search(N{}, 1, {.width = 2}, [](const N&, auto emit) {
    emit(N{1, 1, 99}); emit(N{1, 2, 99});
  });
  assert(deduped.score == 2);
}
void test_chokudai() {
  struct N {
    int depth = 0, score = 0;
    bool operator<(const N& other) const { return score < other.score; }
  };
  N got = ht::chokudai_search(N{}, 3, 3, 2, [](const N& n, auto emit) {
    emit(N{n.depth + 1, n.score}); emit(N{n.depth + 1, n.score + 1});
  });
  assert(got.depth == 3 && got.score == 3);
}
int main() {
  test_rng(); test_structures(); test_incremental_score(); test_beam(); test_chokudai();
  ht::Timer t(1); assert(t.elapsed_exact() >= 0);
  std::cout << "all tests passed\n";
}
