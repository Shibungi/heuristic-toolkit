#include <bits/stdc++.h>
#include "library/structures/zobrist_hash.hpp"
#include "examples/past_ahc/common/ahc004.hpp"

using namespace std;
using namespace past_ahc::ahc004;

// 【問題】AHC004: トーラス盤面に含まれる入力文字列数を最大化する。
// 【この解法】ランダムな1マス変更を試し、covered語数が非悪化かつ未訪問hashなら採用する山登り。
// 【State】Grid、現在covered数、盤面Zobrist hash、訪問済みhash集合。
// 【比較方向】next_score >= currentだけ採用する最大化。同点遷移は許す。
// 【Zobristの役割】マスold→new変更時の盤面hashをO(1)更新し、同じ盤面への往復を避ける。
// 【限界】scoreは全再計算、hash衝突確認なし、30反復だけの教材版で高得点解ではない。
// 公式問題: https://atcoder.jp/contests/ahc004/tasks/ahc004_a
// ZobristHashの使用例: AHC004の盤面を1マス変更したとき、ハッシュをO(1)更新する。
// 探索が同じ盤面を行き来するのを防ぐ。衝突は理論上あるので、厳密性が必要なら盤面比較も足す。
int main() {
  ios::sync_with_stdio(false); cin.tie(nullptr);
  const Input in = read();
  Grid grid(in.n, string(in.n, 'A'));
  ht::ZobristTable table(in.n * in.n, 8, 404);
  ht::Xoshiro256StarStar rng(405);
  uint64_t hash = 0;
  for (int p = 0; p < in.n * in.n; ++p) hash ^= table.at(p, 0);
  unordered_set<uint64_t> visited{hash};
  int current = score(in, grid);

  // score()は説明用の全再計算なので反復数を抑える。差分評価へ替えたら増やしてよい。
  for (int iter = 0; iter < 30; ++iter) {
    int pos = rng.uniform_int(0, in.n * in.n - 1);
    int r = pos / in.n, c = pos % in.n;
    int old_value = grid[r][c] - 'A', new_value = rng.uniform_int(0, 7);
    if (old_value == new_value) continue;
    uint64_t next_hash = hash;
    table.change(next_hash, pos, old_value, new_value);
    if (visited.contains(next_hash)) continue;
    grid[r][c] = char('A' + new_value);
    int next_score = score(in, grid);
    if (next_score >= current) {
      current = next_score; hash = next_hash; visited.insert(hash);
    } else {
      grid[r][c] = char('A' + old_value);  // 不採用時は盤面とハッシュを両方戻す。
    }
  }
  print(grid);
}
