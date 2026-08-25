# C++ library guide

ブラウザで検索・絞り込み・木上Beamの動作図を使う場合は
[`handbook/library.html`](../handbook/library.html) を開いてください。

この文書は `library/` 以下の全headerについて、アルゴリズム、使いどころ、最小の
使い方、計算量・注意点をまとめたものです。APIを覚えることより、問題に合わせて
headerやtemplateを直接改造できることを優先しています。

すべてheader-onlyで、継承frameworkや問題固有のI/Oはありません。

```cpp
#include "library/search/simulated_annealing.hpp"
#include "library/random/rng.hpp"
```

提出時は `tools/make_submission.sh main.cpp submission.cpp` でローカルincludeを展開します。

## 一覧

| Header | 主な用途 |
|---|---|
| `search/simulated_annealing.hpp` | 差分更新型SA、温度schedule、adaptive近傍 |
| `search/hill_climbing.hpp` | 山登り法baseline |
| `search/beam_search.hpp` | Stateコピー型の通常Beam |
| `search/tree_beam_search.hpp` | Euler tourによるStateコピーなしBeam |
| `search/chokudai_search.hpp` | 深さ別priority queueを反復するChokudai search |
| `search/mcts.hpp` | UCB1/PUCT/progressive widening付きMCTS baseline |
| `random/rng.hpp` | SplitMix64 seed展開、xoshiro256** |
| `timer/timer.hpp` | safety marginと間引きclock取得 |
| `structures/fast_clear_array.hpp` | generation counterによるO(1) clear |
| `structures/fast_set.hpp` | dense/sparse set |
| `structures/rollback.hpp` | 値代入のundo log |
| `structures/zobrist_hash.hpp` | 状態hashのO(1)差分更新 |
| `structures/bitset_tools.hpp` | 動的bit列のpopcount/intersection |
| `graph/dsu.hpp` | DSU、rollback DSU |
| `graph/shortest_path.hpp` | multi-source BFS、Dijkstra |
| `grid/grid.hpp` | 2D座標の1次元化と4近傍 |
| `utility/logger.hpp` | release時に消えるdebug log |

---

## Search

### Simulated Annealing — `search/simulated_annealing.hpp`

#### アルゴリズム

最大化でscore差を `delta = new - current`、温度を `T` とすると、改善moveは常に受理し、
悪化moveを確率 `exp(delta / T)` で受理します。序盤に局所最適から抜け、終盤は山登り法に
近づきます。最小化では符号を内部で反転します。

温度は線形または指数scheduleです。`progress` は0から1で、iteration比でも
`Timer::progress()`でも構いません。ライブラリはStateを所有せず、hot loopの
`apply/delta/rollback`を問題側に残します。

#### 最小使用例

```cpp
ht::Timer timer(1.9, 0.03);
ht::Annealer sa({100.0, 0.1, ht::CoolingSchedule::Exponential,
                 ht::Objective::Maximize}, 1234);

while (!timer.poll(iter++)) {
    Move move = propose(state, sa.rng());
    double delta = calc_delta(state, move);
    if (!sa.accept(delta, timer.progress())) continue;
    apply(state, move);
    score += delta;
    if (score > best_score) best_score = score, best = state;
}
```

適用後にしかdeltaが分からない場合はtransaction形を使えます。

```cpp
auto best = ht::run_transactional_sa(
    initial, iterations, sa, propose,
    [](State& s, Move& m) { return apply_and_return_delta(s, m); },
    [](State& s, Move& m) { rollback(s, m); },
    [](uint64_t i, uint64_t n) { return double(i) / n; });
```

複数近傍は `AdaptiveNeighborhoods` で選択し、epochごとに更新します。

```cpp
ht::AdaptiveNeighborhoods neighborhoods(3);
int kind = neighborhoods.choose(sa.rng());
// moveを試した後
neighborhoods.observe(kind, accepted, improved);
if ((iter & 4095) == 0) neighborhoods.update_epoch();
```

#### 注意

- `full_score == incremental_score` をランダム操作で検証する。
- bestは探索用surrogateではなく、可能なら真scoreでも保持する。
- `exp` は悪化moveだけで呼ばれる。clockは毎iteration読まない。
- adaptive近傍は短期間の偶然で確率を0にしない。最低weightを残す。
- restartはbestを保持してStateを再初期化、reheatは `sa.reheat(factor)`。

---

### Hill Climbing — `search/hill_climbing.hpp`

#### アルゴリズム

`delta >= 0` のmoveだけを採用する最大化用local searchです。SAの近傍・deltaをdebugする
baselineとして使います。局所最適から自力では抜けません。

```cpp
State best = ht::hill_climb<State, Move>(
    initial, 1'000'000, 123,
    propose,
    [](const State& s, const Move& m) { return delta_score(s, m); },
    [](State& s, const Move& m) { apply(s, m); });
```

1反復は `propose + delta + 採用時apply`。最小化はscoreの符号を反転するか、問題側loopを
直接書きます。

---

### Normal Beam Search — `search/beam_search.hpp`

#### アルゴリズム

深さごとに全子候補を生成し、評価値上位 `width` 個だけを次の層へ残します。
`nth_element` で上位選択を平均O(C)、Zobrist等のhashで同一状態を除去できます。

```cpp
struct Node {
    State state;
    double score;
    uint64_t hash;
    std::vector<Action> history;
};

Node answer = ht::layered_beam_search(
    initial_node, max_depth,
    {.width = 500, .deduplicate = true, .maximize = true},
    [](const Node& parent, auto emit) {
        for (Action action : actions(parent.state)) {
            Node child = parent;
            apply(child.state, action);
            child.score = evaluate(child.state);
            child.hash = hash(child.state);
            child.history.push_back(action);
            emit(std::move(child));
        }
    });
```

#### 計算量と選択基準

1層の生成候補数をC、State sizeをSとすると、候補生成は概ねO(C·S)、選択は平均O(C)。
Stateが小さい、copyが安い、まず正しいBeamを作りたい場合はこちらを使います。

State copyがprofileを支配したら、次の木上Beamへ移行します。

---

### Tree / Euler-tour Beam Search — `search/tree_beam_search.hpp`

#### アルゴリズム

通常Beamは葉ごとにStateを保持します。木上Beamは、現在の候補の履歴が張る探索部分木を
Euler tourとして保持します。tourの下降辺で `apply(action)`、上昇辺で
`undo(action)` を呼び、1個の可変Stateを各葉へ移動します。

```text
root
 ├─ a ─ c  [leaf 0]
 │    └ d  [leaf 1]
 └─ b ─ e  [leaf 2]

tour: down(a), down(c), leaf, up,
      down(d), leaf, up, up,
      down(b), down(e), leaf, up, up
```

次層で採用された子へ至るpathだけを新しいtourへcommitするため、不採用部分木は自然に
消えます。葉Stateのcopyも、全履歴の重複保存も不要です。

#### Modelの契約

- `Action` は小さく、default constructibleであること。
- `apply(state, action)` と `undo(state, action)` が厳密な逆操作であること。
- `expand(state, emit)` はStateを変更せず、全子について
  `emit(action, child_score, child_hash)` を呼ぶこと。
- 全葉は同じ深さとして扱うfixed-depth Beam。

#### 最小使用例

```cpp
struct Action { int x = 0; };
struct State { int depth = 0; long long score = 0; /* large arrays */ };

ht::TreeBeamSearch<State, Action, long long> beam({
    .width = 5000,
    .deduplicate = true,
    .maximize = true,
});

auto result = beam.solve(
    initial, initial.score, initial_hash, max_depth,
    [](State& state, auto emit) {
        for (Action action : enumerate_actions(state)) {
            // Stateをapplyせずに子の評価/hashを差分予測する
            emit(action,
                 state.score + delta_score(state, action),
                 next_hash(state, action));
        }
    },
    [](State& state, const Action& action) { apply(state, action); },
    [](State& state, const Action& action) { undo(state, action); });

for (Action action : result.actions) output(action);
```

`Result` は `actions`, `score`, `hash`, `depth`, `generated`, `retained` を持ちます。

#### いつ速いか

通常BeamのStateが大きく、Actionとapply/undoが小さい場合に有利です。メモリは
「B個のState」ではなく「採用履歴が張る部分木のAction列」に比例します。全候補がroot直下
のようにprefix共有が少ない場合や、undoが重い場合は利点が減ります。

#### 危険箇所

- undo漏れは別の葉を汚染する。apply回数とundo回数、full recomputationをtestする。
- `expand` 内で一時的にStateを変更したら必ず戻してからreturnする。
- hash衝突時は高score側だけ残る。誤除去不可なら128-bit相当のkeyや追加照合を使う。
- Actionに大きいsnapshotを持たせると木上方式の利点が消える。undoに必要な差分だけ持つ。
- 子scoreを得るため毎回apply/undoするより、可能ならdelta予測を使う。

参考となる方式は Rafael Bocquet の Euler tour beam searchです。AHC021公式解説でも
State copyを避ける実装として紹介されています。出典は `handbook/data/sources.md`。

---

### Chokudai Search — `search/chokudai_search.hpp`

#### アルゴリズム

深さごとにpriority queueを置き、浅い層から順に各queueの上位 `width` 個を展開する操作を
複数sweep繰り返します。通常Beamが各層を一度で捨てるのに対し、未展開候補を深さごとに
残せます。

```cpp
struct Node {
    double score;
    // priority_queueで高評価がtopになる比較
    bool operator<(const Node& other) const { return score < other.score; }
};

Node answer = ht::chokudai_search(
    initial, max_depth, 100 /* sweeps */, 20 /* per-depth width */,
    [](const Node& node, auto emit) {
        for (auto child : expand(node)) emit(std::move(child));
    });
```

良い候補が深さによって不均一に現れる木で候補になります。queue memoryと重複状態の増加に
注意してください。時間制御する場合はこの薄いheaderをコピーし、sweep外側へTimer判定を
追加するのが簡単です。

---

### MCTS — `search/mcts.hpp`

#### アルゴリズム

1 iterationは selection → expansion → rollout → backpropagation。

- UCB1: `mean + C * sqrt(log(parent_visits) / child_visits)`
- PUCT: `mean + C * prior * sqrt(parent_visits) / (1 + child_visits)`
- progressive widening: 訪問数が増えるまで展開child数を制限

```cpp
struct Model {
    std::vector<Action> actions(const State&);
    State next(const State&, Action);
    bool terminal(const State&);
    double evaluate(const State&);       // root視点
    double prior(const State&, Action);
    Action rollout_action(const State&, const std::vector<Action>&,
                          ht::Xoshiro256StarStar&);
};

Action action = ht::mcts<State, Action>(
    root, model,
    {.iterations = 5000, .rollout_depth = 50,
     .exploration = 1.414, .puct = 0.0,
     .progressive_widening_k = 0.0},
    seed);
```

これは読みやすいState-copy baselineです。State遷移が重ければ問題側でapply/undo版へ直接
変えます。分岐巨大・horizon長大・rolloutがnoiseならBeamやgreedyを優先します。
`TranspositionTable` 型は共有統計用の部品ですが、cycle・手番視点・depth依存価値があるため
自動統合していません。

---

## Random and time

### RNG — `random/rng.hpp`

`SplitMix64` はseedを拡散し、`Xoshiro256StarStar` の4語内部状態を初期化します。
xoshiro256**は小さい状態と高速な整数生成を持つ非暗号乱数です。

```cpp
ht::Xoshiro256StarStar rng(12345);
uint64_t raw = rng();
int index = rng.uniform_int(0, n - 1);  // inclusive
uint64_t k = rng.uniform_u64(n);        // [0,n), rejectionでmod bias回避
double x = rng.uniform01();             // [0,1), 53-bit
bool selected = rng.bernoulli(0.2);
```

同じseedなら再現します。暗号用途には使えません。`uniform_u64(0)` は不正です。seedをcase
番号だけにすると入力と乱数が相関する場合があるため、必要なら定数と混ぜます。

### Timer — `timer/timer.hpp`

`steady_clock` を使い、N iterationごとだけ時計を読むことでhot loopのchrono costを
削減します。

```cpp
ht::Timer timer(1.95, 0.03);  // limit, safety margin
for (uint64_t iter = 0; ; ++iter) {
    if (timer.poll(iter, 255)) break;   // 256回に1回update
    double t = timer.progress();        // cached 0..1
}
```

`elapsed()` はcache、`elapsed_exact()` は即時clock取得です。maskは `2^k-1` を想定。
間引き区間中に時間超過する分と出力時間をsafety marginへ含めます。

---

## Structures

### FastClearArray — `structures/fast_clear_array.hpp`

各要素にgeneration stampを持ち、clear時に配列全体を埋めずgenerationを1増やします。

```cpp
ht::FastClearArray<int> dist(n, -1);
dist.set(start, 0);
if (!dist.contains(v)) dist.set(v, next_distance);
int value = dist.get(v);  // 未設定ならdefault値
dist.clear();             // O(1), wrap時だけO(n)
```

メモリO(n)、get/set O(1)、通常clear O(1)。全要素を毎回読む用途には普通のvectorが単純です。

### FastSet — `structures/fast_set.hpp`

universe `[0,n)` に対するdense/sparse setです。dense配列末尾とのswap-removeでinsert、erase、
containsをO(1)、clearをO(1)にします。

```cpp
ht::FastSet active(n);
active.insert(v);
active.erase(v);
if (active.contains(v)) { /* ... */ }
for (int x : active.values()) { /* 順序は不定 */ }
active.clear();
```

key範囲が密に確保できる場合だけ使います。iteration順はeraseで変わります。

### UndoLog — `structures/rollback.hpp`

値を書き換える前のaddress/valueをstackへ保存し、checkpointまでLIFOで戻します。

```cpp
ht::UndoLog<int> undo;
auto cp = undo.checkpoint();
undo.assign(state.x, 10);
undo.assign(state.y, 20);
// 分岐を調べる
undo.rollback(cp);
```

assign/rollbackは変更数に比例。記録中に対象vectorをresizeしてaddressを無効化しないこと。
複数型は型ごとにlogを持つか、問題固有のActionへold valueを保存します。

### Zobrist hash — `structures/zobrist_hash.hpp`

各 `(position,value)` にランダム64-bit値を割り当て、状態hashをXORで構成します。1箇所の
値変更はold/newの2値をXORしてO(1)更新できます。

```cpp
ht::ZobristTable table(positions, value_kinds, 123);
uint64_t hash = 0;
for (int p = 0; p < positions; ++p) hash ^= table.at(p, state[p]);

table.change(hash, p, old_value, new_value);
state[p] = new_value;
```

64-bit衝突確率は低いだけで0ではありません。誤った重複除去が許せない探索では独立seedの
2本、または代表Stateの追加比較を使います。

### Bitset tools — `structures/bitset_tools.hpp`

`vector<uint64_t>` で表した可変長bit列の基本集計です。

```cpp
std::vector<uint64_t> a(words), b(words);
a[v >> 6] |= 1ULL << (v & 63);
int count = ht::popcount(a);
int common = ht::intersection_count(a, b);
```

計算量O(words)。固定長なら `std::bitset<N>` の方が素直です。2配列のword数は同じにします。

---

## Graph and grid

### DSU / RollbackDSU — `graph/dsu.hpp`

通常DSUはunion by size＋path compressionで償却ほぼO(1)。RollbackDSUは過去へ戻すため
path compressionをせず、union by sizeでO(log n)深さを保ちます。

```cpp
ht::DSU dsu(n);
dsu.merge(a, b);
bool connected = dsu.same(a, b);
int component_size = dsu.size(a);

ht::RollbackDSU rollback(n);
auto cp = rollback.checkpoint();
rollback.merge(a, b);
rollback.merge(b, c);
rollback.rollback(cp);
```

RollbackDSUは探索木、offline dynamic connectivity、試行的edge追加に使えます。同じcomponent
のmergeはhistoryへ積まないため、rollbackは必ずcheckpointで行います。

### Shortest path — `graph/shortest_path.hpp`

```cpp
auto distance = ht::bfs(graph, std::vector<int>{source1, source2});

// graph[v] = vector<pair<to, nonnegative_int_cost>>
auto weighted_distance = ht::dijkstra(weighted_graph, source);
```

multi-source BFSはO(V+E)。binary heap DijkstraはO((V+E)log V)、distanceは`long long`。
重み0/1だけなら問題側で0-1 BFSを書く方が速いです。AHC hot loopではqueue/dist workspaceを
再利用する改造を検討します。

### GridIndex — `grid/grid.hpp`

2D gridを`id = r*w+c`へflattenし、連続memoryで保持します。

```cpp
ht::GridIndex grid{h, w};
int v = grid.id(r, c);
for (int d = 0; d < 4; ++d) {
    int nr = r + grid.dr[d], nc = c + grid.dc[d];
    if (grid.inside(nr, nc)) visit(grid.id(nr, nc));
}
```

`row(id)`, `col(id)` もあります。探索反復が多い場合、各idのneighbor listを前計算すると
division/moduloと境界branchを減らせます。

---

## Logging

### Logger — `utility/logger.hpp`

debug buildだけstderrへ出し、releaseでは式ごと消します。

```cpp
HT_LOG("elapsed=" << timer.elapsed()
       << " current=" << score
       << " best=" << best_score);
```

`environment/compile debug` は `HT_ENABLE_LOG` を定義します。releaseではmacro引数の式も
評価されません。大量logはdebug時でも探索挙動を変えるため、iteration maskで間引きます。

---

## 選択の早見表

| 状況 | 第一候補 |
|---|---|
| 完成解を軽い局所変更で改善 | HCで検証 → SA |
| 逐次構築、Stateが小さい | Normal Beam |
| 逐次構築、State copyが支配的、undoが軽い | Tree/Euler-tour Beam |
| 深さごとに良い候補の出現が偏る | Chokudai Search |
| rolloutが価値を予測し、同じnodeを再訪できる | MCTS |
| 密な整数集合を頻繁にclear | FastSet / FastClearArray |
| 探索木で状態変更を戻す | Action固有undo / UndoLog / RollbackDSU |

どの選択でも `quality per iteration`, `iterations per second`, `paired final score` の3つを
測ってください。便利な抽象化より、問題に合わせた薄い改造を優先します。
