#include <bits/stdc++.h>
#include "library/structures/rollback.hpp"

using namespace std;

struct Operation { int r1, c1, r2, c2; };

// UndoLogの使用例: AHC021の三角形を、親 <= 子になるよう隣接swapで改善する。
// 各候補をいったん適用して評価し、悪化時だけrollbackする探索の最小形になっている。
int main() {
  ios::sync_with_stdio(false); cin.tie(nullptr);
  constexpr int N = 30;
  array<array<int, N>, N> a{};
  for (int r = 0; r < N; ++r) for (int c = 0; c <= r; ++c) cin >> a[r][c];
  vector<Operation> answer;
  ht::UndoLog<int> undo(32);

  bool changed = true;
  while (changed && answer.size() < 10000) {
    changed = false;
    for (int r = 0; r + 1 < N && answer.size() < 10000; ++r) {
      for (int c = 0; c <= r && answer.size() < 10000; ++c) {
        // 小さい方の子と比べると、1回のswapで最大の逆転を解消しやすい。
        int child_c = (a[r + 1][c] < a[r + 1][c + 1]) ? c : c + 1;
        auto cp = undo.checkpoint();
        int parent = a[r][c], child = a[r + 1][child_c];
        undo.assign(a[r][c], child);
        undo.assign(a[r + 1][child_c], parent);
        if (parent > child) {
          // 改善手は確定し、古い値をもう使わないのでログを破棄する。
          undo.commit();
          answer.push_back({r, c, r + 1, child_c}); changed = true;
        } else {
          // 評価が悪い候補は盤面を完全に元へ戻す。
          undo.rollback(cp);
        }
      }
    }
  }
  cout << answer.size() << '\n';
  for (auto op : answer) cout << op.r1 << ' ' << op.c1 << ' ' << op.r2 << ' ' << op.c2 << '\n';
}
