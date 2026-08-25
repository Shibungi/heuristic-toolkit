# heuristic-toolkit

AHC / Marathon Matchの開始前に準備しておく、AI不要の薄いC++ライブラリ、コピー用
テンプレート、評価基盤、判断ハンドブックです。既存の問題別コードは変更しません。
標準ライブラリ以外の実行時依存はありません。

## コンテスト開始時

1. `templates/` から問題ディレクトリへコピーする。
2. 公式generator/scorerを用意し、手計算1ケースと一致させる。
3. 最小feasible出力、greedy baselineを作る。
4. 10→100 seedを `tester/run.py` で保存する。
5. profileでscore/transition/copyのbottleneckとiterations/secを確認する。
6. delta randomized testを保ったまま、表現・近傍・探索を一つずつ比較する。

判断に迷ったら `handbook/index.html`、開始直後は
`handbook/cheat-sheet.html` を開きます。両方とも完全オフラインです。
全C++ headerのアルゴリズム・使い方・使用例は `handbook/library.html` にあります。
検索や木上Beamの動作図が不要なら、文章だけの `library/README.md` も使えます。

## ビルド

```bash
environment/compile release main.cpp solver   # AtCoder提出相当の基本flags
environment/compile debug main.cpp solver-dbg # ASan/UBSan/_GLIBCXX_DEBUG/log
environment/compile profile main.cpp solver-p # gprof用
```

現行AtCoderは C++23 / GCC 15.2.0。ローカルWSLのGCC 11.4.0とは異なるため、厳密に
C++23機能を確認するときは `environment/Dockerfile` を使います。完全なAtCoder
library imageではない点と、CPU/OSは再現できない点に注意してください。

## AtCoder提出用の1ファイルを作る

開発中は `#include "library/..."` で分割し、提出直前にローカルincludeだけを展開します。
`<bits/stdc++.h>`、`<atcoder/all>` などの山括弧includeはそのまま残ります。

```bash
tools/make_submission.sh main.cpp submission.cpp
```

このコマンドは再帰bundle、未解決ローカルinclude検査、release compileを順に行います。
生成された `submission.cpp` は編集せず、元コードを直して再生成してください。追加の
include rootが必要なら、そのままbundler引数を渡せます。

```bash
tools/make_submission.sh src/main.cpp submission.cpp -I path/to/include
```

bundler単体は `python3 tools/bundle.py main.cpp -o submission.cpp` です。quoted includeが
見つからなければ黙って残さずエラーにするため、ローカルheaderを含み忘れた提出を防げます。

## SAを使いたい

`templates/sa_template.cpp` をコピーします。`Annealer` は温度とMetropolis判定だけを
担当し、問題固有のState/Moveを隠しません。主力loopは次の形です。

```cpp
Move move = propose(state, sa.rng());
double delta = calc_delta(state, move);
if (sa.accept(delta, timer.progress())) {
    apply(state, move);
    score += delta;
    if (score > best_score) best = state;
}
```

適用後にしかdeltaが分からないmoveには `run_transactional_sa` のapply/rollback形が
あります。複数近傍は `AdaptiveNeighborhoods` で最低確率を残しつつ選択できます。
restartはbestを保持してStateを再初期化、reheatは `sa.reheat()` を使います。

## Beam / 木上Beam / Chokudai / MCTSを使いたい

- Beam: `templates/beam_template.cpp`。Stateが大きい場合は共通関数を捨て、
  `templates/tree_beam_template.cpp` のEuler-tour方式へ移行してください。
- 木上Beam: `library/search/tree_beam_search.hpp`。探索部分木のEuler tourを保持し、
  1個のStateをapply/undoして葉を巡回するため、beam要素ごとのState copyがありません。
- Chokudai: `library/search/chokudai_search.hpp`。深さごとのqueueを繰り返し展開します。
- MCTS: `templates/mcts_template.cpp`。copy型の読みやすいbaselineです。遷移が重いなら
  apply/rollback版を問題側で書きます。rolloutの予測力を小規模A/Bしてから採用します。

## 1000ケースを評価したい（throughput）

```bash
python3 tester/run.py --solver ./solver --input-dir in --cases 0-999 \
  --jobs 6 --cores 0,2,4,6,8,10 --timeout 2.3 \
  --score-command './tools/scorer {input} {output}'
```

caseごとのstdout/stderr/scorer出力と `case,score,time,status,exit_code` CSVを保存します。
`--score-regex` は公式scorerの形式に合わせてください。

## A/B比較したい

```bash
python3 tester/compare.py results/A.csv results/B.csv
# 最小化問題:
python3 tester/compare.py results/A.csv results/B.csv --lower-is-better
```

同じ成功seedのmean/median delta、win/tie/lose、標準偏差、bootstrap 95% CI、妥当な
正値scoreならratio/geometric meanを表示します。

## 実行時間を正確に測りたい

```bash
python3 tester/benchmark.py --solver ./solver --input in/0000.txt \
  --warmup 2 --repeat 15 --core 0
```

単独processで測ります。並列testerのcase時間を提出速度の推定に使わないでください。
詳細は `docs/benchmarking.md`。

## テストとmicro benchmark

```bash
tests/run.sh
benchmark/run_micro.sh
```

unit testにはRNG再現性、dense set、timestamp clear、undo、Zobrist差分、Beam重複、
10万回の `full score == incremental score` を含みます。micro結果は必ず同時に保存される
CPU/compiler metadataと合わせて読み、実探索loopでも効果を再測定します。

## 構成

- `library/`: inlineしやすい薄いheader。継承frameworkなし。
- `templates/`: 本番でコピーし直接壊してよい出発点。
- `tester/`: score throughput、単体benchmark、paired A/B。
- `benchmark/`: micro実験と結果。
- `handbook/`: 全文検索・タグ付き静的HTMLと印刷用1ページ。
- `environment/`: release/debug/profileとGCC 15.2 Docker。
- `tests/`: invariant/randomized tests。
- `docs/repository-audit.md`: 既存資産・不足・重複の調査記録。

## 原則

正しさ → 速度 → 変更容易性 → debug容易性 → 汎用性 → 抽象の美しさ。
最終性能は `quality per iteration × iterations per second`。追加機能は必ず同じseedで
最終scoreまで比較します。本ツールキットのコードはMIT、参照資料は
`handbook/data/sources.md` に出典のみを記録しています。各コンテストの規約を優先し、
コンテスト中に禁止されたAIを呼ぶ機能は一切含みません。
