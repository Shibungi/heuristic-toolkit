# Existing repository audit (2026-08-26)

Target `/home/shibungi/competitive-programming` is a symlink to
`/home/shibungi/competitive_programming`. The root is not a Git repository.

## Present

- Many standalone exact-contest solutions under `atcoder/`, `tenkei90/`, etc.
- A large `marathon/` archive spanning AHC002–AHC067 (with gaps), HTTF/RCO and
  Topcoder MM126/MM130.
- Problem-local Rust visualizers/testers, input seeds, outputs and ad-hoc Python
  multi-test/benchmark scripts.
- Problem-specific annealing, MCTS and random utilities embedded in submissions.
- Recent AHC061/AHC066 work contains more structured scripts and reports.

## Reusable

- Past solution files are valuable as examples of representations, move design and
  bottlenecks.
- Existing official visualizer/scorer directories should remain per-problem; the new
  runner accepts arbitrary scorer commands rather than replacing them.
- Historical seed/output sets can be used to validate the generic runner.

## Missing before this toolkit

- Versioned common C++ heuristic headers and randomized invariant tests.
- A consistent output/status/score CSV schema across contests.
- Explicit separation of throughput testing and stable single-process timing.
- Paired-seed statistical A/B comparison.
- Searchable offline decision handbook and contest cheat sheet.
- Current AtCoder GCC 15.2/C++23 reproduction (local WSL had GCC 11.4.0).

## Duplicated / too problem-specific

- RNG, timer, score logging and multi-test loops recur inside submissions.
- Rust tool layouts and score parsers are tied to each AHC and should not be merged
  into a monolithic framework.
- Old output files and binaries are experimental records, not general libraries.

Decision: leave every existing contest directory untouched and create the independent
`heuristic-toolkit/` sibling. No third-party submission code was copied.
