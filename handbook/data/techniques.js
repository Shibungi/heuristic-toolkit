window.HT_TECHNIQUES = [
  {
    id: "past-modification-greedy", title: "過去改変貪欲", aliases: "過去に戻る prefix replay greedy rollout",
    category: "construction", summary: "一度作った操作列の途中へ戻り、そこで別の選択をして、壊れた後半を貪欲に作り直す。逐次構築問題を局所探索可能な形へ変換するAHC特有の重要パターン。",
    trigger: "1手を変えると以降が全て無効になるため、完成操作列へ普通のswap/insert近傍を定義しにくい。途中状態からは高速なgreedyで必ず完走できる。",
    recipe: ["prefix状態を再現できるようAction列とcheckpointを持つ", "時刻tを選び、元Actionと異なる候補を1つ入れる", "t+1以降は元列を可能なら再生し、無効手はskipまたはrepairする", "残りをgreedy rolloutし、完成解の真scoreで採否を決める", "tの分布、改変候補数、rollout評価をBeam/HC/SAへ拡張する"],
    cost: "素朴には1近傍O(prefix再生+rollout)。snapshot、rollback、永続prefix、木上Beamで共有すると軽くなる。",
    danger: "『過去改変』は万能名ではない。greedy rolloutが弱いと後半の再構築noiseが大きく、prefixを深く変えるほど比較不能になる。",
    contests: "AHC021、AHC042、AHC065、AHC068で系譜を追える。",
    sources: [["AHC021メモ（過去に戻ってワープ）","https://jetbead.github.io/AtCoderHeuristicContestMemo/ContestMemo/ahc021.html"],["AHC042 wata公開提出","https://atcoder.jp/contests/ahc042/submissions/62337792"],["AHC068観戦記","https://chokudai.github.io/atoco-cp/post/2026-07-18-ahc068/"]]
  },
  {
    id: "greedy-rollout", title: "貪欲プレイアウト / completion oracle", aliases: "repair completion greedy",
    category: "construction", summary: "部分解を必ず完成解へ写す軽いgreedyを、初期解だけでなく評価器・repair・探索遷移として使う。",
    trigger: "途中状態の真価値が分からないが、完成させればscoreを測れる。残りを埋める決定的または乱択greedyが十分速い。",
    recipe: ["最小feasible completionを最初に作る", "途中状態→完成scoreの相関と計算時間を測る", "tie-breakを特徴量化し複数policyを用意する", "Beamの葉評価、MCTS rollout、LNS repair、過去改変の後段として共有する"],
    cost: "残りdecision数×候補評価。差分cacheと候補制限が本体。", danger: "同じgreedyだけで完成すると探索の多様性が消える。tie-break乱択と複数policyを比較する。", contests: "AHC014の削除→貪欲再構築など。",
    sources: [["AHC014日記","https://qiita.com/ToastUz/items/39ed10404f14123c6ee2"]]
  },
  {
    id: "priority-search", title: "貪欲の順番・優先度を探索する", aliases: "priority parameter greedy order",
    category: "construction", summary: "複雑な解そのものではなく、greedyがdecisionを確定する順序・tie-break重みを探索変数にする。",
    trigger: "greedyは強いが、早く資源を使った要素だけが損をする。priorityを変えると全体の完成形が大きく変わる。",
    recipe: ["各候補の特徴量と局所評価をログする", "priority=Σw_k feature_kまたは順列をStateにする", "重みmutation/要素swap後にgreedyを再実行", "真scoreでSA/HCし、過学習をholdout seedで確認する"],
    cost: "greedy全再実行が基本。prefix共有や局所repairが可能なら差分化。", danger: "少数seedへのparameter overfit。重みの意味よりpaired holdoutの改善を優先する。", contests: "AHC001の一度目の失敗度から二度目の処理順を決める例。",
    sources: [["AHC001参加記","https://note.com/nyanyan_cubetech/n/ne907cd897457"]]
  },
  {
    id: "randomized-greedy", title: "Randomized Greedy / GRASP", aliases: "rcl top k multistart",
    category: "construction", summary: "常に局所1位を選ばず、上位Kまたは閾値内候補から乱択して、多数の異なる盆地へ入る。",
    trigger: "greedyのtie-breakでscoreが大きく揺れ、1回が軽い。", recipe: ["完全greedyをbaselineにする", "上位K、softmax、RCL閾値を比較", "multi-startでbestを残す", "各候補へ短いlocal searchを付ける"],
    cost: "1 runはgreedyと同程度、総costはrestart数倍。", danger: "乱択を増やすだけでは弱い。score/秒と多様性（hash距離）を記録する。", contests: "多くのconstruction型AHCの初手。", sources: []
  },
  {
    id: "ruin-recreate", title: "部分破壊・再構築（LNS）", aliases: "ruin recreate destroy repair large neighborhood",
    category: "local", summary: "関連するdecision群をまとめて消し、greedy/DP/Beamで作り直して局所moveでは越えにくい谷を越える。",
    trigger: "single moveが制約でほぼ棄却される、または強く結び付いたclusterを同時に変えないと改善しない。",
    recipe: ["空間・時間・依存グラフから破壊領域を選ぶ", "影響外の不変条件を固定", "小さい順にgreedy→限定Beam→exact repairを試す", "破壊サイズを温度/停滞で変える"],
    cost: "破壊量とrepairに比例。改善/秒でサイズを選ぶ。", danger: "大きく壊しすぎると毎回ほぼ初期解。小さすぎると通常近傍と同じ。rollback不変条件をtestする。", contests: "AHC014、AHC059。",
    sources: [["AHC059 2位解法（公式解説欄）","https://atcoder.jp/contests/ahc059/editorial"],["ALNS原論文","https://doi.org/10.1287/trsc.1050.0135"]]
  },
  {
    id: "context-strength", title: "文脈の強さでSA / Beamを選ぶ", aliases: "context locality suffix dependency",
    category: "diagnosis", summary: "解の一部を変えたとき、残りの良さが何%維持されるかで局所探索の成立性を判断する。",
    trigger: "SAかBeamか迷う逐次問題。", recipe: ["良い解へランダムな小変更を入れる", "変更外decisionの有効率・score保持率を測る", "高保持率なら局所探索、低ければ構築探索/過去改変を優先", "境界ではprefix固定+suffix再探索を試す"],
    cost: "小規模probeで測定可能。", danger: "問題全体の一語ラベルではない。時刻や近傍種類ごとに文脈強度が変わる。", contests: "AHC021（強い）、AHC024（弱い）の比較。",
    sources: [["短期AHCで勝つためのテクニック","https://speakerdeck.com/shun_pi/duan-qi-ahcdesheng-tutamenotekunituku"]]
  },
  {
    id: "time-varying-surrogate", title: "時間依存の評価関数 / surrogate", aliases: "dynamic evaluation diversity annealing objective",
    category: "evaluation", summary: "序盤は多様性や各項の均等化を重視し、終盤に真scoreへ連続的に戻す。探索が大きな一項へ早期固定されるのを防ぐ。",
    trigger: "生scoreで探索すると一項だけが支配し、将来必要な候補が消える。", recipe: ["scoreを意味のある項へ分解", "序盤用変換と真scoreの順位相関を測る", "progressで係数を連続補間", "bestは常に真scoreでも別保持"],
    cost: "評価項の差分更新cost。", danger: "surrogate bestを最終出力しない。切替が急だと探索景観が不連続になる。", contests: "AHC047公式解説。",
    sources: [["AHC047公式解説","https://atcoder.jp/contests/ahc047/editorial/13096"]]
  },
  {
    id: "reduced-representation", title: "簡略表現を探索し、詳細を具体化する", aliases: "anchor macro materialize coarse representation",
    category: "representation", summary: "巨大な操作列や経路を直接探索せず、anchor・順序・集合・macroだけを最適化し、詳細はgreedy/DPで生成する。",
    trigger: "詳細Stateが巨大でcopy/遷移が重いが、scoreの大勢を少数decisionが決める。", recipe: ["最終解をmacroとdetailに分解", "macroから常にfeasibleなdetailを生成", "surrogateと真scoreの順位相関を測る", "最後にdetailだけ局所改善"],
    cost: "探索空間縮小と引換えに具体化cost。", danger: "表現が最適解を除外する自主制約になる。複数表現や制約緩和をA/Bする。", contests: "AHC043。",
    sources: [["AHC043公式解説","https://atcoder.jp/contests/ahc043/editorial/12355"]]
  },
  {
    id: "relaxed-subproblem", title: "緩和問題・部分問題の解を評価値にする", aliases: "relaxation dp lower bound potential",
    category: "evaluation", summary: "元問題の制約を一部外したDP/flow/matching等を高速に解き、残りcostの下界・将来価値としてBeamやgreedyへ与える。",
    trigger: "途中状態の単純scoreが将来の詰みや必要手数を反映しない。", recipe: ["難しい制約を1つずつ外す", "緩和解が真値のboundか単なる相関値か区別", "更新範囲をcache", "評価精度向上とnodes/sec低下を同時測定"],
    cost: "部分問題solver次第。二段階評価が有効。", danger: "高精度でも重すぎる評価はBeam幅を殺す。", contests: "AHC033でDPをBeam評価へ使う例。", sources: [["AHC技法メモ","https://hackmd.io/@bin101/rJRBO85ep"]]
  },
  {
    id: "exact-finish", title: "最後だけexact / DP / repair", aliases: "exact finish endgame dynamic programming",
    category: "hybrid", summary: "探索はmacro構造を決め、残った小問題だけDP・全探索・matching等で厳密に仕上げる。",
    trigger: "終盤に変数が少なくなる、または候補を少数残せる。", recipe: ["exact化できる残りサイズをbenchmark", "探索中に複数候補を保持", "制限時間を前半探索/終盤exactへ固定配分", "exact失敗時のfeasible fallbackを残す"],
    cost: "終盤サイズに指数/多項式。", danger: "finish用時間を残せず未完成にしない。", contests: "AHC066のfinal DP。", sources: [["AHC066公式解説","https://atcoder.jp/contests/ahc066/editorial/21433"]]
  },
  {
    id: "tree-beam-technique", title: "木上Beam / Euler-tour Beam", aliases: "tree beam rollback copy free",
    category: "beam", summary: "Action履歴の共有prefixを木として保持し、1個のStateをapply/undoして全葉を展開する。",
    trigger: "State copy bytes×生成nodeが支配的で、Actionが小さく可逆。", recipe: ["通常Beamで正解を作る", "apply/undo randomized test", "保持葉の最小部分木をEuler tour化", "下降apply・上昇undo・葉expand", "copy削減とtour/hash overheadをprofile比較"],
    cost: "生成数は同じ、State memory O(1)+Action tree。", danger: "共有prefixが少ない、Actionがsnapshot並みに大きい、undoが重い場合は勝たない。", contests: "AHC021。", sources: [["AHC021公式解説","https://atcoder.jp/contests/ahc021/editorial/6681"],["Rafael Bocquet実装","https://gitlab.com/rafaelbocquet-cpcontests/euler-tour-beam-search"]]
  },
  {
    id: "candidate-reduction", title: "候補制限・支配関係・二段階評価", aliases: "candidate top k dominance cheap expensive",
    category: "beam", summary: "全Actionを高価に評価せず、安い特徴で上位候補へ絞ってから真評価する。探索の賢さより生成node/秒を増やす。",
    trigger: "分岐が巨大、評価の大半が明らかに弱い候補へ消えている。", recipe: ["cheap scoreとtrue deltaの順位相関を収集", "各親top-K/閾値/bucketを比較", "一部random枠を残し見落としを測る", "K×beam幅を時間から逆算"],
    cost: "cheap全列挙+K件true評価。", danger: "候補制限が自主制約になる。Kを増やしたA/Bとrandom探索枠で検証。", contests: "多くのBeam型AHC。", sources: []
  },
  {
    id: "dedupe-diversity", title: "重複除去と多様性制御", aliases: "zobrist transposition bucket diversity",
    category: "beam", summary: "同値Stateをhashでまとめるだけでなく、似すぎた候補がbeamを占有しないよう特徴量bucketごとに枠を持つ。",
    trigger: "幅を増やしても同型候補ばかりでscoreが伸びない。", recipe: ["正しい同値性keyを定義", "64-bit collision riskを許容可能か決める", "score以外の構造特徴でbucket", "global topとbucket枠を混ぜる"],
    cost: "hash期待O(1)、bucket数管理。", danger: "異なる将来を同一hashへ潰す誤設計は衝突より深刻。depth依存情報もkeyへ入れる。", contests: "Beam/Chokudai全般。", sources: []
  },
  {
    id: "phase-scheduling", title: "探索の段階化（macro→micro）", aliases: "phase time allocation nested search coarse to fine",
    category: "hybrid", summary: "前半は大構造や強いmove、後半は順序・細部・repairへ時間と近傍頻度を移す。",
    trigger: "異なる時間尺度の変数を同じ近傍確率で動かすと互いを壊す。", recipe: ["変数をmacro/micro/endgameへ分類", "各phaseの目的と真score bestを分離", "時間境界を固定してまずA/B", "停滞や温度で滑らかに頻度変更"],
    cost: "各phase solverの合計。", danger: "複雑なschedulerより、固定2〜3段階の方がdebugしやすい。", contests: "AHC066。", sources: [["AHC066公式解説","https://atcoder.jp/contests/ahc066/editorial/21433"]]
  },
  {
    id: "local-recompute", title: "影響groupだけ真値再計算", aliases: "delta local group dependency reverse evaluation",
    category: "speed", summary: "変更が少数groupに閉じるなら、複雑な閉形式deltaを無理に作らず、そのgroupだけ真scoreを再実行する。",
    trigger: "full scoreは重いが、moveで変わる依存成分を列挙でき、各groupは小さい。", recipe: ["score=Σgroup contributionへ分解", "move前の影響group寄与を引く", "State変更後に同groupだけ再計算", "定期full scoreで一致検証", "dependencyが短い前向き/逆向きを比較"],
    cost: "影響group数×group再計算。", danger: "cache invalidation漏れ。random move列で毎回fullとの一致をtest。", contests: "AHC049。", sources: [["AHC049公式解説","https://atcoder.jp/contests/ahc049/editorial/13384"]]
  },
  {
    id: "feasible-encoding", title: "常にfeasibleな表現 / repair付きmove", aliases: "constraint preserving encoding feasibility",
    category: "representation", summary: "不正解を大量生成して弾くのでなく、表現やAction自体でhard constraintを守る。守れない部分だけ局所repairする。",
    trigger: "近傍の大半が制約違反、終盤repair不能。", recipe: ["hard/soft constraintを分離", "保存量を保つcycle・exchange・Kempe chainを探す", "違反原因別の棄却率をログ", "soft化するなら終了前repair可能性を証明/検証"],
    cost: "候補生成がやや重くてもfeasible率向上で回収。", danger: "feasible表現が探索空間を分断することがある。複数の大近傍を用意。", contests: "彩色・matching・割当型。", sources: []
  },
  {
    id: "local-sufficient-check", title: "局所の十分条件で重い制約判定を省く", aliases: "local connectivity sufficient condition 3x3",
    category: "speed", summary: "正確な全体判定より厳しいが安い局所条件を先に使い、通った候補だけ必要に応じて真判定する。",
    trigger: "連結性/BFS等が1moveごとに支配的で、変更が局所。", recipe: ["変更領域周辺の危険patternを列挙", "false positiveを出さない十分条件として実装", "不明な場合だけfull check", "拒否しすぎによる探索空間損失をA/B"],
    cost: "固定領域ならO(1)。", danger: "必要条件と十分条件を取り違えない。厳しすぎる条件で良解を除外する。", contests: "AHC023の局所連結チェック例。", sources: [["AHC技法メモ","https://hackmd.io/@bin101/rJRBO85ep"]]
  },
  {
    id: "portfolio-selection", title: "入力分布に合わせたsolver portfolio", aliases: "algorithm selection generated instances portfolio",
    category: "experiment", summary: "複数の独立方針を用意し、同じ入力parameterで事前生成したinstance群の平均実績から採用方針を選ぶ。",
    trigger: "入力parameterから向くalgorithmが変わるが、単一caseの特徴量だけでは決めにくい。", recipe: ["方針A/Bを同じ時間で実装", "同parameterの入力を多数生成", "holdout上の期待score/失敗率を比較", "本番入力parameterで選択または短時間両方実行"],
    cost: "事前/実行時portfolio数倍。", danger: "generatorと本番分布の一致、選択に使ったseedへのoverfit。", contests: "AHC025のLDM/LPT選択例。", sources: [["AHC技法メモ","https://hackmd.io/@bin101/rJRBO85ep"]]
  },
  {
    id: "multi-move-escape", title: "1変数近傍で詰むなら2変数・chainを試す", aliases: "two cell move compound neighborhood escape",
    category: "local", summary: "単一変更では必ず悪化/違反する局所障壁を、2点同時変更や連鎖moveで越える。",
    trigger: "特定seedで局所解が再現し、single moveの改善候補が0。", recipe: ["詰み状態を可視化", "最小何変数なら制約を保って変えられるか分析", "single moveを主力にcompoundを低頻度追加", "近傍別改善/秒とbest寄与をログ"],
    cost: "候補数・delta範囲が増える。", danger: "大近傍だけにすると反復数が落ちる。", contests: "AHC024の1マス/2マス変更。", sources: [["wata氏AHC024解説投稿を含む技法メモ","https://hackmd.io/@bin101/rJRBO85ep"]]
  },
  {
    id: "multistart", title: "Multi-start / restart / reheating", aliases: "restart reheat basin",
    category: "local", summary: "同じ長い探索1回と、異なる初期解からの短い探索複数回を比較し、盆地依存を減らす。",
    trigger: "seed間/乱数間のbest分散が大きく、早期に改善が止まる。", recipe: ["best-so-farを探索外に保持", "初期解・RNG・greedy tie-breakを変える", "各runの立上り曲線を記録", "restart回数と1run長をpaired比較", "reheatはStateを保つ中間案"],
    cost: "総時間をrunへ分割。", danger: "各runが立上り前に終わると悪化。", contests: "AHC020でSA複数回の例。", sources: [["AHC020 WA_TLE解説","https://atcoder.jp/contests/ahc020/editorial/6584"]]
  },
  {
    id: "adaptive-neighborhood", title: "近傍のadaptive選択", aliases: "bandit neighborhood reward",
    category: "local", summary: "近傍ごとの採用率・改善率・costを測り、探索中に選択weightを更新する。",
    trigger: "phaseやseedにより有効近傍が変わる。", recipe: ["最低確率を残す", "accepted/improvedだけでなくns/試行も記録", "epoch単位で平滑更新", "固定確率baselineとpaired比較"],
    cost: "近傍数O(K)選択、統計O(1)。", danger: "短期の偶然で有効近傍を0にしない。報酬scaleを調整。", contests: "汎用SA/LNS。", sources: []
  },
  {
    id: "workspace-reuse", title: "timestamp・bitboard・workspace再利用", aliases: "fast clear bitset bfs cache precomputed neighbors",
    category: "speed", summary: "探索アルゴリズムを変える前に、繰返し初期化・allocation・隣接生成を消して反復数を増やす。",
    trigger: "profileでmemset/vector allocation/BFS setupが上位。", recipe: ["generation counterでvisited clearをO(1)化", "gridをflattenしneighbors事前計算", "密集合はbitset/dense-sparse set", "queue/dist/candidate vectorをloop外へ", "実loopで再計測"],
    cost: "メモリ増、generation wrap処理。", danger: "micro benchmarkだけで採用しない。STLの素直な実装が支配的でなければ維持する。", contests: "AHC014のrotated bitboardなど。", sources: [["AHC技法メモ","https://hackmd.io/@bin101/rJRBO85ep"]]
  },
  {
    id: "hybrid-pipeline", title: "Construction → Beam → SA の役割分担", aliases: "hybrid greedy beam annealing",
    category: "hybrid", summary: "Beamで多様な構築候補、SAで完成解の微調整、exact/repairで終盤を仕上げる。各手法を万能化せず強い局面だけ担当させる。",
    trigger: "単一手法では初期解依存または終盤の細部が弱い。", recipe: ["greedy baseline", "短いBeamで構造が異なる上位候補を複数保存", "各候補へ時間制限SA", "真score bestを共有", "最後にrepair/exact"],
    cost: "総時間の配分問題。", danger: "各段の受渡し表現変換が重い。Beam上位が似るなら多様性枠を入れる。", contests: "AHC043。", sources: [["AHC043公式解説","https://atcoder.jp/contests/ahc043/editorial/12355"]]
  },
  {
    id: "expensive-subproblem-separation", title: "高価な従属問題を探索loopから分離", aliases: "steiner network deferred exact cache",
    category: "hybrid", summary: "主decisionを動かすたび高価なnetwork/DPを解かず、近似・cache・定期評価で代用し、最後に真の従属問題を解く。",
    trigger: "scoreが主変数と高価な最適化subproblemからなり、後者が反復数を殺す。", recipe: ["subproblem入力のhash cache", "安いbound/surrogateを普段使用", "一定間隔または有望解だけ真評価", "最終候補にexact/高精度法"],
    cost: "真評価頻度で調整。", danger: "surrogate順位相関が低いと間違った主変数を探索する。", contests: "AHC020の塔出力とSteiner tree分離。", sources: [["AHC020公式/ユーザー解説","https://atcoder.jp/contests/ahc020/editorial/6584"]]
  }
];
