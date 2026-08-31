# AHC070 pre-contest workspace

このディレクトリは2026-08-29 15:00 JSTより前に生成・公開する、問題非依存の作業台です。
コンテスト開始後は生成AIを使わず、本人が問題文を読んで `main.cpp` を編集してください。

> `main.cpp` の現在の内容は、コンテスト終了後の2026-09-01に追加した振り返り用Beam実装です。
> コンテスト前に公開されていたscaffoldとは異なり、終了済みAHC070の問題固有コードです。

## コンテスト後に追加したBeam実装

現在の `main.cpp` は、10,000手を一度に探索せず、短いBeamを繰り返す
receding-horizon方式です。先読み結果の先頭2手だけを確定し、その時点の距離場から再探索します。

Beamの評価値は小さいほど良く、次の3項です。

```text
評価値 = 確定コスト + 未来コスト + 重複配置ペナルティ

確定コスト = 既に発生した怪異についての公式危険度の合計
未来コスト = Σ 危険度(t_怪異) × 0.9995^(t_怪異 - t_now)
重複配置ペナルティ = 既に札があるマスへ置いた回数 × 0.25
```

未来の各怪異の危険度は、現在の札集合から怪異位置までの最短マンハッタン距離を使って
`floor(distance × sqrt(t_怪異 + 1))` と見積もります。遠い未来ほど、その間に新しい札で
改善できる可能性が高いため、指数的に割り引きます。

距離場 `dist[cell]` は札追加時にBFSで小さくなるセルだけ更新し、変更箇所をjournalへ記録します。
Beamの別枝へ移るときはjournalを逆順に戻すため、10,000要素のStateコピーは行いません。
時間切れ間際は1手greedyへ切り替え、必ず10,000手を出力します。

主要な調整箇所は `main.cpp` 冒頭の `FUTURE_DECAY`、`OVERLAP_PENALTY`、
`MAX_LOOKAHEAD`、`NORMAL_BEAM_WIDTH`、`COMMIT_STEPS` です。

## 開始直後

1. 公式配布物を `tools/`、生成入力を `in/` に置く。
2. 入出力と最小valid解を実装する。
3. `./build.sh debug` でassert・sanitizer付きビルドを行い、バグを確認する。
4. `HT_MODE=debug HT_TIMEOUT=30 ./run.sh in/0000.txt` でdebug実行する。
5. `./build.sh release` の後に `./run.sh in/0000.txt` で提出相当の時間を測る。
6. scorerを目視確認してから複数seedのbaselineを保存する。
7. 一変更ずつA/B比較し、終了20分前から大改造しない。
8. `./submit.sh` でローカルincludeを展開した `submission.cpp` を作る。

## buildとrunの役割

- `debug`: ASan、UBSan、`_GLIBCXX_DEBUG`、assert、ログ付き。バグ発見用。遅くて正常。
- `release`: `-O2 -DNDEBUG`。提出速度・スコア比較用。
- `run.sh`: 実行前にmodeとtimeoutを表示し、1秒ごとに生存を報告する。stderrも隠さない。
- 既定timeoutは5秒。長くする場合は `HT_TIMEOUT=30 ./run.sh in/0000.txt` とする。

「遅い」と思ったら、まず表示されたmodeを見る。debugの時間を提出速度と比較しない。
releaseでも遅い場合は、深さ×幅×遷移数、State copy、全再計算、出力flushをprofileする。

`run.sh`自身が行うのはsolverの1回起動だけである。実行中の待ち時間は`main.cpp`内部の処理であり、
探索のphaseや反復回数まで自動では分からない。必要ならsolverからstderrへ、経過時間、反復回数、
best scoreを0.2〜1秒間隔で出す。毎反復出力すると、ログ自体がbottleneckになるので避ける。

`Timer(1.85, 0.05)`のような停止条件があれば、release版でも意図的に約1.8秒使う。
さらに1反復が重く、時刻確認を数反復ごとにしか行わない場合は、その1反復ぶん停止時刻を超過する。

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
