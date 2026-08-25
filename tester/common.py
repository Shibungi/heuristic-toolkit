from __future__ import annotations

import math
import re
import shlex
import statistics
from pathlib import Path
from typing import Iterable


def parse_cases(spec: str) -> list[int]:
    result: set[int] = set()
    for part in spec.split(","):
        part = part.strip()
        if not part:
            continue
        if "-" in part:
            left, right = part.split("-", 1)
            a, b = int(left), int(right)
            if b < a:
                raise ValueError(f"descending range: {part}")
            result.update(range(a, b + 1))
        else:
            result.add(int(part))
    return sorted(result)


def command_tokens(command: str, **values: object) -> list[str]:
    return [token.format(**values) for token in shlex.split(command)]


def percentile(values: Iterable[float], p: float) -> float:
    xs = sorted(values)
    if not xs:
        return math.nan
    position = (len(xs) - 1) * p
    lo = int(position)
    hi = min(lo + 1, len(xs) - 1)
    return xs[lo] + (xs[hi] - xs[lo]) * (position - lo)


def summary(values: list[float]) -> dict[str, float]:
    mean = statistics.fmean(values)
    return {
        "n": len(values),
        "mean": mean,
        "median": statistics.median(values),
        "p25": percentile(values, 0.25),
        "p75": percentile(values, 0.75),
        "min": min(values),
        "max": max(values),
        "cv": statistics.stdev(values) / mean if len(values) > 1 and mean else 0.0,
    }


def extract_score(text: str, pattern: str) -> float | None:
    matches = re.findall(pattern, text, flags=re.MULTILINE)
    if not matches:
        return None
    value = matches[-1]
    if isinstance(value, tuple):
        value = value[-1]
    return float(value)


def case_path(input_dir: Path, pattern: str, seed: int) -> Path:
    return input_dir / pattern.format(seed=seed)
