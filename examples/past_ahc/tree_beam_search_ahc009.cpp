#include <bits/stdc++.h>
#include "library/search/tree_beam_search.hpp"
#include "examples/past_ahc/common/ahc009.hpp"

using namespace std;
using namespace past_ahc::ahc009;

struct State { int r = 0, c = 0, depth = 0; };
struct Action { int d = 0, from_r = 0, from_c = 0, to_r = 0, to_c = 0; };

// 【問題】AHC009: 確率的移動失敗のある迷路で、200手の到達確率・到達時刻を改善する。
// 【この解法】位置だけの小さい教材Stateを使い、60層・幅300で-distanceを最大化する。
// 【目的関数】child_score = -Manhattan distance。ゴール到達には+1000し、大きい候補を残す。
// 【State/Action】Stateは座標とdepth。Actionは変更前後座標を持ち、applyとundoを完全に対称にする。
// 【TreeBeamの役割】可変State 1個をEuler tourで全葉へ動かし、best Action列を返す。
// 【重要な限界】この実装は深さごとに保持部分木を走査するため、深さ10,000級ではO(width×depth²)化し得る。
// 公式問題: https://atcoder.jp/contests/ahc009/tasks/ahc009_a
// 木上Beamの使用例: AHC009の位置Stateを1個だけ持ち、Euler tourでapply/undoする。
// 実際に到達確率400要素をStateへ入れるとcopyが重くなるため、この方式の利点が大きくなる。
int main() {
  ios::sync_with_stdio(false); cin.tie(nullptr);
  const Input in = read();
  ht::TreeBeamConfig cfg; cfg.width = 300; cfg.deduplicate = true;
  ht::TreeBeamSearch<State, Action> beam(cfg);
  State initial{in.sr, in.sc, 0};
  auto result = beam.solve(initial, -double(manhattan(in, in.sr, in.sc)),
      uint64_t(in.sr * W + in.sc), 60,
      [&](const State& s, auto emit) {
        for (int d = 0; d < 4; ++d) {
          auto [nr, nc] = moved(in, s.r, s.c, d);
          double score = -manhattan(in, nr, nc) + ((nr == in.tr && nc == in.tc) ? 1000 : 0);
          uint64_t hash = uint64_t(s.depth + 1) * 401 + nr * W + nc;
          emit(Action{d, s.r, s.c, nr, nc}, score, hash);
        }
      },
      [](State& s, const Action& a) { s.r = a.to_r; s.c = a.to_c; ++s.depth; },
      [](State& s, const Action& a) { s.r = a.from_r; s.c = a.from_c; --s.depth; });
  string path; for (const Action& a : result.actions) path += DIR[a.d];
  print_200(path);
}
