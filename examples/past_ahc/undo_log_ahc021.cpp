#include <bits/stdc++.h>
#include "library/structures/rollback.hpp"

using namespace std;

struct Operation { int r1, c1, r2, c2; };

// 【問題】AHC021: 三角形の親<=子という条件を、隣接swap最大10,000回で満たし、手数を減らす。
// 【この解法】親より小さい子を見つけたらswapするheap化貪欲。違反数を減らす方向へ進む。
// 【State】三角配列aと出力Operation列。1候補で変更するのは親・子の2整数だけ。
// 【UndoLogの役割】checkpoint後に2代入を記録し、採用ならcommit、不採用ならrollbackする。
// 【採否】parent>childなら局所違反が1つ解消するため採用。それ以外は元へ戻す。
// 【限界】全体違反数の厳密差分や最小手数を評価しない。rollback APIの制御フローを示す例。
// 公式問題: https://atcoder.jp/contests/ahc021/tasks/ahc021_a
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
