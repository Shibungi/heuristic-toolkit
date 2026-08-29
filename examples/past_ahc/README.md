# 過去AHCを題材にしたライブラリ使用例

各ファイルは、該当ヘッダのAPIを実際のAHC入出力へつないだ、単独で提出形式まで動く
教育用baselineです。高得点解の再現ではなく「どこへライブラリを差し込むか」を明確にする
ことを優先しています。提出前には必ず公式問題文・制約・最新ルールを再確認してください。

| ライブラリ | 題材 | 使用例 |
|---|---|---|
| `timer/timer.hpp` | AHC002 | 制限時間付きランダム再始動 |
| `random/rng.hpp` | AHC002 | seed固定ランダム化貪欲 |
| `structures/fast_set.hpp` | AHC002 | 使用済みタイル集合 |
| `grid/grid.hpp` | AHC002 | 二次元座標の一次元化 |
| `utility/logger.hpp` | AHC002 | release時に消える探索ログ |
| `structures/bitset_tools.hpp` | AHC004 | 収録済み文字列集合の集計 |
| `structures/zobrist_hash.hpp` | AHC004 | 1マス変更の差分ハッシュ |
| `search/simulated_annealing.hpp` | AHC004 | 1マス変更焼きなまし |
| `search/hill_climbing.hpp` | AHC004 | 非悪化1マス変更 |
| `structures/fast_clear_array.hpp` | AHC009 | 再利用可能なBFS距離配列 |
| `graph/shortest_path.hpp` | AHC009 | 決定論的最短路baseline |
| `search/beam_search.hpp` | AHC009 | 層別経路Beam |
| `search/tree_beam_search.hpp` | AHC009 | apply/undo型の木上Beam |
| `search/chokudai_search.hpp` | AHC009 | 深さ別priority queue探索 |
| `search/mcts.hpp` | AHC009 | 次の一手のMCTS |
| `graph/dsu.hpp` | AHC007 | オンラインMSTと仮採用rollback |
| `structures/rollback.hpp` | AHC021 | swap候補の試行・巻き戻し |

## コンパイル

リポジトリルートで次を実行します。

```bash
for source in examples/past_ahc/*.cpp; do
  g++ -std=c++20 -O2 -I. "$source" -o /tmp/"$(basename "$source" .cpp)"
done
```

提出時に利用する場合は、コンテスト開始前に公開された固定コミットの該当ファイルURLを
提出コードへコメントで記載してください。各例は問題固有部分を学ぶための資料であり、
現在開催中の問題を生成AIへ入力したり、生成AIに改善させたりする用途ではありません。
