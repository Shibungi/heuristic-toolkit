#include <bits/stdc++.h>
#include "library/graph/dsu.hpp"

using namespace std;

// 【問題】AHC007: 辺の真の長さが順番に判明するオンラインMSTで、採用辺長合計を最小化する。
// 【この解法】閉路を作らず、実長が座標距離推定の2.2倍+20以下なら辺を採用するbaseline。
// 【目的】最終的に連結を保ちながら総辺長を小さくしたい最小化。ただしこの閾値貪欲は連結を保証しない。
// 【DSUの役割】採用済み辺の連結成分を管理し、閉路になる辺を拒否する。
// 【RollbackDSUの役割】辺を仮採用→連結変化を確認→rollbackする先読みの最小形を示す。
// 【出力】各辺へ0/1を即時flush。インタラクティブなので後の辺を先読みできない。
// 公式問題: https://atcoder.jp/contests/ahc007/tasks/ahc007_a
// DSU / RollbackDSUの使用例: AHC007（オンライン最小全域木）。
// 入力で与えられる400頂点・1995辺について、辺長が判明するたび採否を返す。
// ここでは「閉路を作らず、実長が座標距離から大きく外れない」辺を採用するbaseline。
int main() {
  ios::sync_with_stdio(false); cin.tie(nullptr);
  constexpr int N = 400, M = 1995;
  vector<int> x(N), y(N);
  for (int i = 0; i < N; ++i) cin >> x[i] >> y[i];
  vector<int> u(M), v(M);
  for (int i = 0; i < M; ++i) cin >> u[i] >> v[i];

  ht::DSU forest(N);
  ht::RollbackDSU trial(N);
  for (int i = 0; i < M; ++i) {
    int actual_length; cin >> actual_length;
    const double estimated = hypot(x[u[i]] - x[v[i]], y[u[i]] - y[v[i]]);

    // RollbackDSUでは、複数辺を仮採用する高度な先読みも同じ形で書ける。
    // この最小例では1辺だけ試し、閉路ならmerge()がfalseになることを確認して戻す。
    auto checkpoint = trial.checkpoint();
    bool connects_components = trial.merge(u[i], v[i]);
    trial.rollback(checkpoint);

    bool take = connects_components && !forest.same(u[i], v[i]) &&
                actual_length <= estimated * 2.2 + 20.0;
    if (take) { forest.merge(u[i], v[i]); trial.merge(u[i], v[i]); }
    cout << int(take) << endl;  // endlでflushし、インタラクティブジャッジへ即時送信する。
  }
}
