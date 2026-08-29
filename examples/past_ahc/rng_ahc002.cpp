#include <bits/stdc++.h>
#include "library/random/rng.hpp"
#include "examples/past_ahc/common/ahc002.hpp"

using namespace std;
using namespace past_ahc::ahc002;

// 【問題】AHC002: 同じタイルを再訪しない経路の、通過マス得点合計を最大化する。
// 【この解法】合法4方向を毎手shuffleし、最初の方向へ進む経路を3000回作って最大scoreを返す。
// 【State】座標、使用済みタイル、経路、score。乱数seedは20210822に固定する。
// 【RNGの役割】方向順序だけを変える。同じbinary・入力・seedなら同じbest経路を再現できる。
// 【読む順】外側restart→内側while→Fisher-Yates→合法手選択→score加算→best比較。
// 【限界】点数を次手選択に使わない。再現性とA/B比較のための最小例である。
// 公式問題: https://atcoder.jp/contests/ahc002/tasks/ahc002_a
// RNGの使用例: AHC002で同一seedなら完全に同じ経路を再現できるランダム化貪欲。
// スコア比較時に乱数列を固定できることが、近傍や評価関数のA/B比較では重要になる。
int main() {
  ios::sync_with_stdio(false); cin.tie(nullptr);
  const Input in = read();
  ht::Xoshiro256StarStar rng(20210822);
  string best; long long best_score = -1;
  for (int restart = 0; restart < 3000; ++restart) {
    vector<char> used(2500);
    int r = in.sr, c = in.sc; used[in.tile[r][c]] = true;
    string path; long long score = in.point[r][c];
    while (true) {
      array<int, 4> dirs{0, 1, 2, 3};
      // uniform_int()を使ったFisher-Yates。std::shuffleの実装差に依存しない。
      for (int i = 3; i > 0; --i) swap(dirs[i], dirs[rng.uniform_int(0, i)]);
      int chosen = -1;
      for (int d : dirs) {
        int nr = r + DR[d], nc = c + DC[d];
        if (inside(nr, nc) && !used[in.tile[nr][nc]]) { chosen = d; break; }
      }
      if (chosen < 0) break;
      r += DR[chosen]; c += DC[chosen]; used[in.tile[r][c]] = true;
      score += in.point[r][c]; path += DIR[chosen];
    }
    if (score > best_score) best_score = score, best = path;
  }
  print(best);
}
