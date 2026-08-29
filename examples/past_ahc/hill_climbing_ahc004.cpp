#include <bits/stdc++.h>
#include "library/search/hill_climbing.hpp"
#include "examples/past_ahc/common/ahc004.hpp"

using namespace std;
using namespace past_ahc::ahc004;

struct State {
  const Input* in = nullptr; Grid grid;
  double score() const { return past_ahc::ahc004::score(*in, grid); }
};
struct Move { int r, c; char after; };

// 山登り法の使用例: AHC004でスコアを悪化させない1マス変更だけを受理する。
// SA版との違いを同じState/Moveで比較でき、温度が本当に必要かを切り分けられる。
int main() {
  ios::sync_with_stdio(false); cin.tie(nullptr);
  const Input in = read();
  State initial{&in, Grid(in.n, string(in.n, 'A'))};
  auto best = ht::hill_climb<State, Move>(
      initial, 30, 4005,
      [&](const State&, ht::Xoshiro256StarStar& rng) {
        return Move{rng.uniform_int(0, in.n - 1), rng.uniform_int(0, in.n - 1),
                    char('A' + rng.uniform_int(0, 7))};
      },
      [](const State& s, const Move& mv) {
        State next = s; next.grid[mv.r][mv.c] = mv.after;
        return next.score() - s.score();
      },
      [](State& s, const Move& mv) { s.grid[mv.r][mv.c] = mv.after; });
  print(best.grid);
}
