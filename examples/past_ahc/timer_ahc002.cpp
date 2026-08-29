#include <bits/stdc++.h>
#include "library/timer/timer.hpp"
#include "library/random/rng.hpp"
#include "examples/past_ahc/common/ahc002.hpp"

using namespace std;
using namespace past_ahc::ahc002;

// Timerの使用例: AHC002のランダム化貪欲を、提出時間ぎりぎりまで再始動する。
// 時間制限を直接while条件にせずpoll()を使うことで、時計取得の重さを間引く。
int main() {
  ios::sync_with_stdio(false); cin.tie(nullptr);
  const Input in = read();
  ht::Timer timer(1.85, 0.05);  // 50msは出力・環境差の安全余裕として残す。
  ht::Xoshiro256StarStar rng(2);
  string best;
  long long best_score = in.point[in.sr][in.sc];

  for (uint64_t trial = 0; !timer.poll(trial, 63); ++trial) {
    vector<char> used(2500);
    int r = in.sr, c = in.sc;
    used[in.tile[r][c]] = true;
    long long score = in.point[r][c];
    string path;
    while (true) {
      vector<int> candidate;
      for (int d = 0; d < 4; ++d) {
        int nr = r + DR[d], nc = c + DC[d];
        if (inside(nr, nc) && !used[in.tile[nr][nc]]) candidate.push_back(d);
      }
      if (candidate.empty()) break;
      // 高得点マスを好むが、再始動ごとに少し揺らして局所解を変える。
      int d = candidate[rng.uniform_u64(candidate.size())];
      for (int e : candidate)
        if (in.point[r + DR[e]][c + DC[e]] + rng.uniform_int(0, 20) >
            in.point[r + DR[d]][c + DC[d]] + rng.uniform_int(0, 20)) d = e;
      r += DR[d]; c += DC[d]; used[in.tile[r][c]] = true;
      score += in.point[r][c]; path += DIR[d];
    }
    if (score > best_score) best_score = score, best = path;
  }
  print(best);
}
