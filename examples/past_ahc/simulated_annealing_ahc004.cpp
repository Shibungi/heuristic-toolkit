#include <bits/stdc++.h>
#include "library/search/simulated_annealing.hpp"
#include "examples/past_ahc/common/ahc004.hpp"

using namespace std;
using namespace past_ahc::ahc004;

struct State {
  const Input* in = nullptr;
  Grid grid;
  double score() const { return past_ahc::ahc004::score(*in, grid); }
};
struct Move { int r, c; char before, after; };

// 【問題】AHC004: 盤面に含まれる入力文字列数を最大化する。
// 【目的関数】State::score() = covered語数。大きいほど良く、SAConfig既定のMaximizeを使う。
// 【近傍】ランダムな1マスをA〜Hへ変更。delta = 変更後covered数 − 変更前covered数。
// 【採否】delta>=0は必ず、delta<0も温度依存確率で受理し、局所最大からの脱出を試す。
// 【返り値】最後のcurrentではなく、探索中にcovered数が最大だったbest Grid。
// 【限界】deltaを全再計算する30反復の説明版。実戦では影響する行・列だけを差分評価する。
// 公式問題: https://atcoder.jp/contests/ahc004/tasks/ahc004_a
// 焼きなましの使用例: AHC004の1マス変更。説明優先で差分ではなく全再計算する。
// 実戦では変更行・列に関係する単語だけ再評価し、1近傍をO(M)未満へ落とす。
int main() {
  ios::sync_with_stdio(false); cin.tie(nullptr);
  const Input in = read();
  State initial{&in, Grid(in.n, string(in.n, 'A'))};
  ht::SAConfig cfg; cfg.start_temperature = 3.0; cfg.end_temperature = 0.05;
  ht::Annealer sa(cfg, 4004);
  // 全再計算版は教育用なので短く回す。差分評価を実装した場合のみ反復数を増やす。
  constexpr uint64_t ITER = 30;
  auto best = ht::run_delta_sa<State, Move>(
      initial, ITER, sa,
      [&](const State& s, ht::Xoshiro256StarStar& rng) {
        int r = rng.uniform_int(0, in.n - 1), c = rng.uniform_int(0, in.n - 1);
        return Move{r, c, s.grid[r][c], char('A' + rng.uniform_int(0, 7))};
      },
      [&](const State& s, const Move& mv) {
        State next = s; next.grid[mv.r][mv.c] = mv.after;
        return next.score() - s.score();
      },
      [](State& s, const Move& mv) { s.grid[mv.r][mv.c] = mv.after; },
      [](uint64_t i, uint64_t n) { return double(i) / n; });
  print(best.grid);
}
