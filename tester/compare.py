#!/usr/bin/env python3
from __future__ import annotations

import argparse
import csv
import math
import random
import statistics
from pathlib import Path

from common import percentile


def read_scores(path: Path) -> dict[int, float]:
    with path.open(newline="") as f:
        return {int(row["case"]): float(row["score"]) for row in csv.DictReader(f)
                if row.get("status") == "ok" and row.get("score") not in (None, "", "None")}


def bootstrap_ci(values: list[float], samples: int = 10000, seed: int = 1) -> tuple[float, float]:
    rng = random.Random(seed); n = len(values)
    estimates = [statistics.fmean(values[rng.randrange(n)] for _ in range(n)) for _ in range(samples)]
    return percentile(estimates, 0.025), percentile(estimates, 0.975)


def main() -> None:
    p = argparse.ArgumentParser(description="Paired A/B score comparison")
    p.add_argument("a", type=Path); p.add_argument("b", type=Path)
    p.add_argument("--lower-is-better", action="store_true"); p.add_argument("--bootstrap", type=int, default=10000)
    args = p.parse_args(); a, b = read_scores(args.a), read_scores(args.b); cases = sorted(a.keys() & b.keys())
    if not cases: raise SystemExit("no paired successful cases")
    deltas = [b[c] - a[c] for c in cases]
    signed = [-x for x in deltas] if args.lower_is_better else deltas
    ratios = [b[c] / a[c] for c in cases if a[c] > 0 and b[c] > 0]
    wins = sum(x > 0 for x in signed); ties = sum(x == 0 for x in signed); losses = len(signed) - wins - ties
    lo, hi = bootstrap_ci(signed, args.bootstrap)
    print(f"paired cases       : {len(cases)}\nwin/tie/lose       : {wins}/{ties}/{losses}\n"
          f"mean signed delta  : {statistics.fmean(signed):.6g}\nmedian signed delta: {statistics.median(signed):.6g}\n"
          f"delta stddev       : {statistics.stdev(signed) if len(signed) > 1 else 0:.6g}\n"
          f"bootstrap 95% CI   : [{lo:.6g}, {hi:.6g}]")
    if ratios:
        oriented = [1 / r for r in ratios] if args.lower_is_better else ratios
        print(f"mean ratio B/A     : {statistics.fmean(oriented):.6g}\nmedian ratio B/A   : {statistics.median(oriented):.6g}\n"
              f"geometric mean     : {math.exp(statistics.fmean(math.log(r) for r in oriented)):.6g}")


if __name__ == "__main__": main()
