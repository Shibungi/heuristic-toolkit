#include <bits/stdc++.h>
#include "library/search/beam_search.hpp"
#include "examples/past_ahc/common/ahc009.hpp"

using namespace std;
using namespace past_ahc::ahc009;

struct Node {
  int r = 0, c = 0; string path; double score = 0; uint64_t hash = 0;
};

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
