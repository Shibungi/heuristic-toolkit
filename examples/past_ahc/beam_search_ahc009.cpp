#include <bits/stdc++.h>
#include "library/search/beam_search.hpp"
#include "examples/past_ahc/common/ahc009.hpp"

using namespace std;
using namespace past_ahc::ahc009;

struct Node {
  int r = 0, c = 0; string path; double score = 0; uint64_t hash = 0;
};

// 【問題】AHC009: 200文字の移動指示を出し、確率的移動失敗下で早くゴールするほど高得点。
// 【この解法】60手の各深さで位置Stateを展開し、マンハッタン距離が小さい上位200件を残す。
// 【目的関数】Node.score = -distance。BeamConfig.maximize=trueなので、0に近い大きな値ほど良い。
// 【重複】depthと位置からhashを作り、同一深さ・同一位置ではscore最大の1件だけ残す。
// 【返り値】深さ60のbest Nodeとpath。残りはUで埋め、出力形式を200文字にする。
// 【限界】pと位置分布を無視し、Nodeごとにpath文字列をcopyする。正解戦略の完成版ではない。
// 公式問題: https://atcoder.jp/contests/ahc009/tasks/ahc009_a
// 通常Beamの使用例: AHC009で「ゴールへ近く、壁衝突の少ない」経路を層ごとに残す。
// 本番では確率pを使った到達分布をStateに持たせ、期待到達確率をscoreにする。
int main() {
  ios::sync_with_stdio(false); cin.tie(nullptr);
  const Input in = read();
  Node initial{in.sr, in.sc, "", -double(manhattan(in, in.sr, in.sc)),
               uint64_t(in.sr * W + in.sc)};
  ht::BeamConfig cfg; cfg.width = 200; cfg.deduplicate = true;
  Node best = ht::layered_beam_search(initial, 60, cfg, [&](const Node& node, auto emit) {
    for (int d = 0; d < 4; ++d) {
      auto [nr, nc] = moved(in, node.r, node.c, d);
      Node child{nr, nc, node.path + DIR[d], -double(manhattan(in, nr, nc)),
                 uint64_t((node.path.size() + 1) * 401 + nr * W + nc)};
      // 壁へ当たる手は合法だが、この単純モデルでは小さな罰を与える。
      if (nr == node.r && nc == node.c) child.score -= 0.25;
      emit(std::move(child));
    }
  });
  print_200(best.path);
}
