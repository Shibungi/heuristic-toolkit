#include <bits/stdc++.h>
#include "library/search/chokudai_search.hpp"
#include "examples/past_ahc/common/ahc009.hpp"

using namespace std;
using namespace past_ahc::ahc009;

struct Node {
  int r = 0, c = 0; string path; double score = 0;
  bool operator<(const Node& other) const { return score < other.score; }
};

// 【問題】AHC009: 失敗確率のある移動指示列を作り、ゴールへ早く到達させる。
// 【この解法】各深さのpriority_queueへ位置Stateを残し、8 sweep×各層30件を展開する。
// 【目的関数】score=-distance（ゴールは+1000）。Node::operator<により大きいscoreがtopになる最大化。
// 【通常Beamとの差】1回の層展開で捨てず、浅い層の次点候補を後のsweepで再び展開できる。
// 【返り値】深さ60のqueue.top()。最終層が空だと使えないため全Stateから4Actionを生成する。
// 【限界】pを無視し、同一位置の重複除去もない。探索器の制御フローを読むための例。
// 公式問題: https://atcoder.jp/contests/ahc009/tasks/ahc009_a
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
