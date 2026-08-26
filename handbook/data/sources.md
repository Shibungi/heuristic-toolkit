# Sources and provenance

Checked 2026-08-26. The handbook paraphrases and generalizes these sources; it does not
copy third-party implementations. Toolkit code is independently written under MIT.

- [AtCoder 2025-10 languages and libraries](https://img.atcoder.jp/file/language-update/2025-10/language-list.html): C++23 GCC 15.2.0, flags, library versions.
- [AtCoder C++23 environment update, 2026-06-16](https://atcoder.jp/posts/language_update20260616_en): removal of `-flto=auto` effective 2026-06-15.
- [Introduction to Heuristics Contest official editorial](https://img.atcoder.jp/intro-heuristics/editorial.pdf): greedy, local search, SA and apply/undo basics.
- [AHC049 editorial](https://atcoder.jp/contests/ahc049/editorial/13384): group-local recalculation, relocate/swap, reverse-direction evaluation.
- [AHC047 editorial](https://atcoder.jp/contests/ahc047/editorial/13096): time-varying surrogate objective to preserve diversity early and approach the raw score late.
- [AHC043 editorial](https://atcoder.jp/contests/ahc043/editorial/12355): optimize a reduced representation, greedily materialize details, beam-generated candidates feeding SA.
- [AHC020 editorials](https://atcoder.jp/contests/ahc020/editorial/6575): add/remove/exchange neighborhoods and separating an expensive network subproblem.
- [AHC059 editorial](https://atcoder.jp/contests/ahc059/editorial/15029): permutation representation and linked-list updates for insertion moves.
- [AHC066 editorial](https://atcoder.jp/contests/ahc066/editorial/21433): staged time allocation, nested hill climbing and final exact DP repair.
- [AHC021 editorial](https://atcoder.jp/contests/ahc021/editorial/6681): Euler-tour beam search used to avoid State copies; reports a problem-specific 2x–5x improvement.
- [Rafael Bocquet's Euler tour beam search](https://gitlab.com/rafaelbocquet-cpcontests/euler-tour-beam-search): original explanation, progressively optimized implementations and benchmarks (MIT).
- [Topcoder Marathon Match introduction](https://www.topcoder.com/blog/marathon-match-challenges-introduction): local visualizer, iterative measurement, and server/local timing caveat.
- [Topcoder: Approaching a Marathon Match](https://www.topcoder.com/blog/approaching-marathon-match-task-pt-1): problem-first workflow and approximation mindset.

AHC archive and field techniques:

- [AtCoder official contest archive](https://atcoder.jp/contests/archive?category=0&ratedType=4): canonical past-contest set; AHC001--AHC069 had ended as of 2026-08-26.
- [AtCoder Heuristic Contest Analyzer](https://shindannin.github.io/ac_analyzer/): independent cross-contest index of tasks, top submissions, AHC radio and participant write-ups. The local archive links to it but does not copy its content.
- Shun_PI, [短期AHCで勝つためのテクニック](https://speakerdeck.com/shun_pi/duan-qi-ahcdesheng-tutamenotekunituku): context strength, search-method selection, and avoiding premature self-imposed constraints.
- [AHC021 contest memo](https://jetbead.github.io/AtCoderHeuristicContestMemo/ContestMemo/ahc021.html): changing a past decision and greedily replaying the suffix ("過去に戻ってワープ/過去改変").
- [AHC042 public submission #62337792](https://atcoder.jp/contests/ahc042/submissions/62337792): wata's public implementation referenced by later participants as a past-modification greedy example.
- [AHC068 recap](https://chokudai.github.io/atoco-cp/post/2026-07-18-ahc068/): reports the lineage from AHC042/AHC065 to past-modification greedy plus beam search in AHC068.
- [AHC practical technique notes](https://hackmd.io/@bin101/rJRBO85ep): participant-maintained links for local sufficient checks, bitboards, multi-cell moves, relaxed subproblems and input-matched solver selection.
- [AHC014 diary](https://qiita.com/ToastUz/items/39ed10404f14123c6ee2): greedy playout and partial delete/rebuild neighborhood.
- [AHC001 write-up](https://note.com/nyanyan_cubetech/n/ne907cd897457): a first greedy pass used to determine the priority order of a second pass.
- [AtCoder's explanation of AHC](https://info.atcoder.jp/overview/contest/heuristic): recommends post-contest review via participant posts and links collected in each contest's editorial tab.
- [AHCの楽しみ方](https://img.atcoder.jp/ahf2-a7k3m9q2/ahf002-2.pdf): AHC radio with wata/kaede/writer and reuse of knowledge from past tasks.

Algorithm background:

- Kocsis & Szepesvari, [Bandit based Monte-Carlo Planning](https://link.springer.com/chapter/10.1007/11871842_29), 2006 (UCT).
- Rosin, [Multi-armed Bandits with Episode Context](https://www.aaai.org/ocs/index.php/AAAI/AAAI11/paper/view/3561), 2011 (PUCT family).
- Ropke & Pisinger, [An Adaptive Large Neighborhood Search Heuristic](https://doi.org/10.1287/trsc.1050.0135), 2006.
