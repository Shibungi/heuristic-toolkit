#include <bits/stdc++.h>
#include "library/structures/fast_set.hpp"
#include "examples/past_ahc/common/ahc002.hpp"

using namespace std;
using namespace past_ahc::ahc002;

// 【問題】AHC002: タイル再訪禁止を守りながら、訪問マス得点を最大化する。
// 【この解法】現在位置から合法な隣接マスのうち、そのマス自身の点が最大の方向へ進む貪欲。
// 【State】座標と経路。FastSetには既に踏んだタイルIDを入れる。
// 【比較方向】best_valueが大きい隣接マスを選ぶので最大化。全経路scoreの先読みはしない。
// 【FastSetの役割】contains/insertをO(1)で行い、再訪禁止というfeasible条件だけを管理する。
// 【限界】一手先評価なので早く行き止まりになり得る。解法性能ではなく集合APIの教材。
// 公式問題: https://atcoder.jp/contests/ahc002/tasks/ahc002_a
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
