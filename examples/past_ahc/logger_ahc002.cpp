#include <bits/stdc++.h>
#include "library/utility/logger.hpp"
#include "examples/past_ahc/common/ahc002.hpp"

using namespace std;
using namespace past_ahc::ahc002;

// Loggerの使用例: AHC002の貪欲解を壊さずにデバッグ情報だけをstderrへ出す。
// 提出時はHT_ENABLE_LOGを定義しないため、HT_LOG内の式そのものが消える。
int main() {
  ios::sync_with_stdio(false); cin.tie(nullptr);
  const Input in = read();
  vector<char> used(2500);
  int r = in.sr, c = in.sc, score = in.point[r][c];
  used[in.tile[r][c]] = true;
  string answer;
  while (true) {
    int chosen = -1;
    for (int d = 0; d < 4; ++d) {
      int nr = r + DR[d], nc = c + DC[d];
      if (inside(nr, nc) && !used[in.tile[nr][nc]] &&
          (chosen < 0 || in.point[nr][nc] > in.point[r + DR[chosen]][c + DC[chosen]])) chosen = d;
    }
    if (chosen < 0) break;
    r += DR[chosen]; c += DC[chosen]; used[in.tile[r][c]] = true;
    score += in.point[r][c]; answer += DIR[chosen];
    HT_LOG("step=" << answer.size() << " pos=(" << r << ',' << c << ") score=" << score);
  }
  HT_LOG("final_score=" << score);
  print(answer);
}
