window.HT_AHC_ARCHIVE = {
  checked: "2026-08-26",
  latestPast: 69,
  spotlights: {
    1: {title:"AtCoder Ad / Rectangles", tags:["greedy","sa","priority"], note:"一度目の貪欲で苦しい長方形を測り、二度目は苦しい順に資源を確保する。貪欲の順序を探索する発想の入口。"},
    4: {title:"String Packer", tags:["placement","sa","wata-writer"], note:"文字列配置・重なり・局所変更。wata氏writer回として、問題設定と公式解説の設計意図も読む。"},
    9: {title:"Robust Memory of Commuting Routes", tags:["expectation","dp","sa"], note:"確率的に壊れる操作列。シミュレーションだけでなく期待値DPを評価へ落とす例。"},
    14:{title:"Rect Join", tags:["greedy-rollout","lns","bitboard"], note:"操作を削除して依存部分を再構築する近傍、貪欲playout、高速bitboardの代表例。"},
    19:{title:"Silhouette Block Puzzle Creation", tags:["geometry","matching","multistart"], note:"2投影を同時に満たす立体構築。対応付け、連結部品、複数初期解を考える。"},
    20:{title:"Broadcasting", tags:["sa","steiner","decomposition"], note:"塔の出力とnetwork構築を分離し、高価なSteiner近似を探索の最後またはcache付きで扱う。"},
    21:{title:"Pyramid Sorting", tags:["tree-beam","past-modification","greedy"], note:"Euler-tour木上Beamと『途中へ戻って別手を入れ、後半を貪欲に作り直す』発想の重要回。"},
    23:{title:"Crops on Grid", tags:["feasibility","local-check","planning"], note:"長期間の栽培計画と通路制約。局所の十分条件で高価な連結性判定を減らす例。"},
    24:{title:"Topological Map", tags:["sa","compound-move","connectivity"], note:"1マス変更で詰む局面を2マス同時変更で越える。近傍が探索空間を連結にするかを見る。"},
    25:{title:"Balancing by Balance", tags:["query","portfolio","offline-generation"], note:"同じ入力parameterで問題を多数生成し、複数algorithmの期待性能から本番方針を選ぶ。"},
    26:{title:"Stack of Boxes", tags:["coarse-to-fine","greedy","grouping"], note:"要素を組にして実効Nを下げるなど、入力を扱いやすいmacro単位へ変換する。"},
    33:{title:"Container Handling with Cranes", tags:["beam","relaxation","dp"], note:"制約を緩めた部分問題DPを将来costの評価としてBeamへ組み込む。"},
    34:{title:"Leveling with Excavator", tags:["greedy","route","inventory"], note:"運搬量と移動を同時に扱う。まず単純greedyで構造を可視化し、経路・積載の改善へ進む。"},
    36:{title:"Efficient Signal Control", tags:["representation","sequence","beam"], note:"巨大な詳細列を直接探索せず、信号配列や経由点など重要変数へ表現を落とす。"},
    38:{title:"Tree Robot Arm", tags:["beam","chokudai","design"], note:"ロボットアーム構造の設計と操作列を分離。通常Beam/Chokudai searchの公開参加記を比較する。"},
    42:{title:"Oni wa Soto, Fuku wa Uchi", tags:["past-modification","greedy","wata-code"], note:"wata氏の公開提出 #62337792 が、後のAHC065/068へ再利用された過去改変貪欲の読み物。"},
    43:{title:"Railway Company", tags:["representation","beam","sa","hybrid"], note:"駅・線路の簡略表現、greedy具体化、Beamで初期候補を作ってSAへ渡すhybrid。"},
    47:{title:"Lovely Language Model", tags:["surrogate","diversity","time-dependent"], note:"序盤の評価を均し候補を温存し、終盤に真scoreへ戻す時間依存surrogate。"},
    49:{title:"Durability-Constrained Transport", tags:["delta","local-recompute","sa"], note:"変更が少数groupに閉じるとき、groupだけ真値を再計算し、依存方向も比較する。"},
    59:{title:"Stack to Match Pairs", tags:["lns","destroy-repair","route"], note:"完成解の一部を破壊して再構築する強い近傍。破壊サイズとrepair品質/秒を読む。"},
    65:{title:"Conveyor Design", tags:["past-modification","greedy","wata-writer"], note:"AHC042型の過去改変貪欲が再利用された回。wata氏writer解と参加者の実装差を追う。"},
    66:{title:"AtCoder Heuristic Contest 066", tags:["phase","chokudai","tsp","exact-finish"], note:"macro作成、順序最適化、終盤DPという段階的時間配分の例。"},
    68:{title:"Rectangle Swap", tags:["past-modification","beam","wata-writer"], note:"過去改変貪欲をBeam化した方針が上位で共有された回。AHC042→065→068の系譜で読む。"}
  }
};
