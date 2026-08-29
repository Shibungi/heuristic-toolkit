# AHC070 pre-contest workspace

このディレクトリは2026-08-29 15:00 JSTより前に生成・公開する、問題非依存の作業台です。
コンテスト開始後は生成AIを使わず、本人が問題文を読んで `main.cpp` を編集してください。

## 開始直後

1. 公式配布物を `tools/`、生成入力を `in/` に置く。
2. 入出力と最小valid解を実装する。
3. `./build.sh debug` でassert・sanitizer付きビルドを行う。
4. `./run.sh in/0000.txt` で1ケースを確認する。
5. scorerを目視確認してから複数seedのbaselineを保存する。
6. 一変更ずつA/B比較し、終了20分前から大改造しない。
7. `./submit.sh` でローカルincludeを展開した `submission.cpp` を作る。

## 公開URL

- Toolkit固定コミット: https://github.com/Shibungi/heuristic-toolkit/tree/2d90ab3f4d7c05ad2434ec5ff0767fa07009c64b
- ライブラリ説明: https://github.com/Shibungi/heuristic-toolkit/blob/2d90ab3f4d7c05ad2434ec5ff0767fa07009c64b/handbook/library.html
- 過去AHC使用例: https://github.com/Shibungi/heuristic-toolkit/tree/2d90ab3f4d7c05ad2434ec5ff0767fa07009c64b/examples/past_ahc

提出にコピーしたAI生成済みコードには、該当する固定ファイルURLをコメントで記載します。
`main` URLではなく、開始前のコミットハッシュを含むURLを使ってください。

## 時間配分メモ

- 0–20分: 理解、入出力、最小valid解、手計算1ケース
- 20–50分: greedy baseline、100 seed評価、ボトルネック確認
- 50–180分: 最も効く改善を一つずつ比較
- 180–220分: 安定化、worst seed確認、提出候補固定
- 220–240分: release build、提出、出力形式の最終確認
