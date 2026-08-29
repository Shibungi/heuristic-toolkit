#include <bits/stdc++.h>
#include "library/structures/fast_set.hpp"
#include "examples/past_ahc/common/ahc002.hpp"

using namespace std;
using namespace past_ahc::ahc002;

// FastSetの使用例: AHC002の「同じタイル番号を二度踏まない」をO(1)で管理する。
// vector<bool>でも十分だが、erase()を使うDFSや再利用時にも同じAPIで扱える。
int main() {
  ios::sync_with_stdio(false); cin.tie(nullptr);
  const Input in = read();
  ht::FastSet used(2500);
  int r = in.sr, c = in.sc; used.insert(in.tile[r][c]);
  string answer;
  while (true) {
    int best_d = -1, best_value = -1;
    for (int d = 0; d < 4; ++d) {
      int nr = r + DR[d], nc = c + DC[d];
      if (!inside(nr, nc) || used.contains(in.tile[nr][nc])) continue;
      // 一手先の点だけを見る単純な貪欲。ライブラリの最小使用例として意図的に薄い。
      if (in.point[nr][nc] > best_value) best_value = in.point[nr][nc], best_d = d;
    }
    if (best_d < 0) break;
    r += DR[best_d]; c += DC[best_d];
    used.insert(in.tile[r][c]); answer += DIR[best_d];
  }
  print(answer);
}
