#include <bits/stdc++.h>
#include "library/search/chokudai_search.hpp"
#include "examples/past_ahc/common/ahc009.hpp"

using namespace std;
using namespace past_ahc::ahc009;

struct Node {
  int r = 0, c = 0; string path; double score = 0;
  bool operator<(const Node& other) const { return score < other.score; }
};

// Chokudai Searchの使用例: AHC009で各深さの有望状態を複数回に分けて展開する。
// 通常Beamと異なり、浅い層の次点候補がqueueに残るため、早い誤選択を後のsweepで救える。
int main() {
  ios::sync_with_stdio(false); cin.tie(nullptr);
  const Input in = read();
  Node initial{in.sr, in.sc, "", -double(manhattan(in, in.sr, in.sc))};
  Node best = ht::chokudai_search(initial, 60, 8, 30, [&](const Node& node, auto emit) {
    for (int d = 0; d < 4; ++d) {
      auto [nr, nc] = moved(in, node.r, node.c, d);
      double value = -manhattan(in, nr, nc) - 0.01 * node.path.size();
      if (nr == in.tr && nc == in.tc) value += 1000;  // 到達後も強く残す。
      emit(Node{nr, nc, node.path + DIR[d], value});
    }
  });
  print_200(best.path);
}
