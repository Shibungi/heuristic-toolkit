#!/usr/bin/env python3
from __future__ import annotations

import argparse
import json
import os
import subprocess
import time
from pathlib import Path

from common import command_tokens, summary


def one_run(command: str, input_path: Path, timeout: float, core: int | None) -> float:
    tokens = command_tokens(command, input=input_path, seed=0, output="/dev/null")
    with input_path.open("rb") as stdin:
        start = time.perf_counter()
        proc = subprocess.Popen(tokens, stdin=stdin, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
        if core is not None and hasattr(os, "sched_setaffinity"):
            try: os.sched_setaffinity(proc.pid, {core})
            except (OSError, PermissionError): pass
        proc.wait(timeout=timeout)
        elapsed = time.perf_counter() - start
    if proc.returncode:
        raise RuntimeError(f"solver exited with {proc.returncode}")
    return elapsed


def main() -> None:
    p = argparse.ArgumentParser(description="Single-process stable timing benchmark")
    p.add_argument("--solver", required=True); p.add_argument("--input", type=Path, required=True)
    p.add_argument("--warmup", type=int, default=2); p.add_argument("--repeat", type=int, default=10)
    p.add_argument("--timeout", type=float, default=10); p.add_argument("--core", type=int)
    p.add_argument("--json", type=Path, default=Path("benchmark/results/latest.json")); args = p.parse_args()
    for _ in range(args.warmup): one_run(args.solver, args.input, args.timeout, args.core)
    values = [one_run(args.solver, args.input, args.timeout, args.core) for _ in range(args.repeat)]
    stats = summary(values); stats["samples"] = values; stats["core"] = args.core
    args.json.parent.mkdir(parents=True, exist_ok=True); args.json.write_text(json.dumps(stats, indent=2))
    print(f"median = {stats['median']:.4f} sec\np25    = {stats['p25']:.4f} sec\np75    = {stats['p75']:.4f} sec\n"
          f"min/max= {stats['min']:.4f}/{stats['max']:.4f} sec\nCV     = {stats['cv'] * 100:.2f}%")


if __name__ == "__main__": main()
