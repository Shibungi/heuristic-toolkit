#include <bits/stdc++.h>
#include "library/grid/grid.hpp"
#include "examples/past_ahc/common/ahc002.hpp"

using namespace std;
using namespace past_ahc::ahc002;

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
