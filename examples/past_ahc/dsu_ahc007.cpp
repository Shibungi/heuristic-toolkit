#include <bits/stdc++.h>
#include "library/graph/dsu.hpp"

using namespace std;

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
