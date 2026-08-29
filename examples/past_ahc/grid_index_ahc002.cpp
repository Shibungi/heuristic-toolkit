#include <bits/stdc++.h>
#include "library/grid/grid.hpp"
#include "examples/past_ahc/common/ahc002.hpp"

using namespace std;
using namespace past_ahc::ahc002;

// 【問題】AHC002: 50x50盤面でタイル再訪を避け、訪問得点を最大化する。
// 【この解法】一手先の得点が最大の合法手を繰り返す。評価は大きいほど良い。
// 【State】現在位置をint posに一次元化し、row/colは必要な時だけ復元する。
// 【GridIndexの役割】id/row/col/insideと共通4方向を提供する。探索戦略は決めない。
// 【読む順】posから座標復元→4近傍→境界とタイル検査→最大点方向→pos更新。
// 【限界】FastSet版と同じ単純貪欲。一次元Stateへ変える利点だけを比較する例。
// 公式問題: https://atcoder.jp/contests/ahc002/tasks/ahc002_a
// GridIndexの使用例: AHC002の二次元座標を一次元IDに統一する。
// 状態配列を一次元化すると、探索中の添字計算・コピー・ハッシュが単純になる。
int main() {
  ios::sync_with_stdio(false); cin.tie(nullptr);
  const Input in = read();
  const ht::GridIndex grid{N, N};
  vector<char> used_tile(2500);
  int pos = grid.id(in.sr, in.sc); used_tile[in.tile[in.sr][in.sc]] = true;
  string answer;
  while (true) {
    int chosen = -1, value = -1;
    const int r = grid.row(pos), c = grid.col(pos);
    for (int d = 0; d < 4; ++d) {
      int nr = r + ht::GridIndex::dr[d], nc = c + ht::GridIndex::dc[d];
      if (!grid.inside(nr, nc) || used_tile[in.tile[nr][nc]]) continue;
      if (in.point[nr][nc] > value) value = in.point[nr][nc], chosen = d;
    }
    if (chosen < 0) break;
    int nr = r + ht::GridIndex::dr[chosen], nc = c + ht::GridIndex::dc[chosen];
    pos = grid.id(nr, nc); used_tile[in.tile[nr][nc]] = true; answer += DIR[chosen];
  }
  print(answer);
}
