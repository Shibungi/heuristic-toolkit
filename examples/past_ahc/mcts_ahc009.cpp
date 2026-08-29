#include <bits/stdc++.h>
#include "library/search/mcts.hpp"
#include "examples/past_ahc/common/ahc009.hpp"

using namespace std;
using namespace past_ahc::ahc009;

struct State { int r = 0, c = 0, depth = 0; };

struct Model {
  const Input& in;
  vector<int> actions(const State&) const { return {0, 1, 2, 3}; }
  State next(State s, int d) const { tie(s.r, s.c) = moved(in, s.r, s.c, d); ++s.depth; return s; }
  bool terminal(const State& s) const { return s.depth >= 30 || (s.r == in.tr && s.c == in.tc); }
  double evaluate(const State& s) const { return (s.r == in.tr && s.c == in.tc) ? 1000 - s.depth : -manhattan(in, s.r, s.c); }
  double prior(const State& s, int d) const {
    auto [nr, nc] = moved(in, s.r, s.c, d);
    return 1.0 / (1.0 + manhattan(in, nr, nc));
  }
  int rollout_action(const State& s, const vector<int>& actions, ht::Xoshiro256StarStar& rng) const {
    int best = actions[rng.uniform_u64(actions.size())];
    for (int d : actions) {
      auto [br, bc] = moved(in, s.r, s.c, best); auto [nr, nc] = moved(in, s.r, s.c, d);
      if (manhattan(in, nr, nc) < manhattan(in, br, bc)) best = d;
    }
    return best;
  }
};

// 【問題】AHC009: 確率的な移動失敗がある迷路で、200手の指示列を決める。
// 【この解法】現在位置から次の1ActionをMCTSで選び、決定論的に動いたと仮定して最大60回繰り返す。
// 【目的関数】Model::evaluateはゴールなら1000-depth、未到達なら-distance。大きいほど良い最大化。
// 【1反復】UCB/PUCT選択→未試行Action展開→最大30手rollout→evaluate→rootまで平均値更新。
// 【返り値】root直下で訪問回数最大の1Action。完全pathではないのでmain側で反復する。
// 【限界】本来の確率分布をStateに持たず、実際の失敗もsampleしない。MCTS API理解用baseline。
// 公式問題: https://atcoder.jp/contests/ahc009/tasks/ahc009_a
// MCTSの使用例: AHC009で次の一手だけを探索し、実際に進んだと仮定して再探索する。
// 確率的な移動結果を厳密に扱うなら、Stateを位置分布にしnext()で確率質量を遷移させる。
int main() {
  ios::sync_with_stdio(false); cin.tie(nullptr);
  const Input in = read(); Model model{in}; State state{in.sr, in.sc, 0};
  ht::MCTSConfig cfg; cfg.iterations = 250; cfg.rollout_depth = 30; cfg.puct = 1.2;
  string path;
  for (int step = 0; step < 60 && (state.r != in.tr || state.c != in.tc); ++step) {
    State root = state; root.depth = 0;
    int action = ht::mcts<State, int>(root, model, cfg, 9000 + step);
    path += DIR[action]; tie(state.r, state.c) = moved(in, state.r, state.c, action);
  }
  print_200(path);
}
