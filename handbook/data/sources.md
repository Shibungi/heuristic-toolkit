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
- [Topcoder Marathon Match introduction](https://www.topcoder.com/blog/marathon-match-challenges-introduction): local visualizer, iterative measurement, and server/local timing caveat.
- [Topcoder: Approaching a Marathon Match](https://www.topcoder.com/blog/approaching-marathon-match-task-pt-1): problem-first workflow and approximation mindset.

Algorithm background:

- Kocsis & Szepesvari, [Bandit based Monte-Carlo Planning](https://link.springer.com/chapter/10.1007/11871842_29), 2006 (UCT).
- Rosin, [Multi-armed Bandits with Episode Context](https://www.aaai.org/ocs/index.php/AAAI/AAAI11/paper/view/3561), 2011 (PUCT family).
- Ropke & Pisinger, [An Adaptive Large Neighborhood Search Heuristic](https://doi.org/10.1287/trsc.1050.0135), 2006.
