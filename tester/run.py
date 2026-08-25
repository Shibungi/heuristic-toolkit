#!/usr/bin/env python3
from __future__ import annotations

import argparse
import csv
import os
import signal
import subprocess
import time
from concurrent.futures import ThreadPoolExecutor, as_completed
from dataclasses import asdict, dataclass
from pathlib import Path

from common import case_path, command_tokens, extract_score, parse_cases


@dataclass
class Result:
    case: int
    score: float | None
    time: float
    status: str
    exit_code: int | None


def terminate(proc: subprocess.Popen[bytes]) -> None:
    try:
        if os.name == "posix":
            os.killpg(proc.pid, signal.SIGKILL)
        else:
            proc.kill()
    except ProcessLookupError:
        pass


def execute(seed: int, args: argparse.Namespace, core: int | None) -> Result:
    input_path = case_path(args.input_dir, args.input_pattern, seed)
    case_dir = args.output_dir / f"{seed:04d}"
    case_dir.mkdir(parents=True, exist_ok=True)
    output_path, stderr_path = case_dir / "stdout.txt", case_dir / "stderr.txt"
    if not input_path.exists():
        return Result(seed, None, 0.0, "missing_input", None)
    tokens = command_tokens(args.solver, seed=seed, input=input_path, output=output_path)
    with input_path.open("rb") as stdin:
        started = time.perf_counter()
        proc = subprocess.Popen(tokens, stdin=stdin, stdout=subprocess.PIPE, stderr=subprocess.PIPE,
                                start_new_session=(os.name == "posix"))
        if core is not None and hasattr(os, "sched_setaffinity"):
            try:
                os.sched_setaffinity(proc.pid, {core})
            except (OSError, PermissionError):
                pass
        try:
            stdout, stderr = proc.communicate(timeout=args.timeout)
            elapsed = time.perf_counter() - started
        except subprocess.TimeoutExpired:
            terminate(proc); stdout, stderr = proc.communicate()
            elapsed = time.perf_counter() - started
            output_path.write_bytes(stdout); stderr_path.write_bytes(stderr)
            return Result(seed, None, elapsed, "timeout", None)
    output_path.write_bytes(stdout); stderr_path.write_bytes(stderr)
    if proc.returncode:
        return Result(seed, None, elapsed, "crash", proc.returncode)
    score_text = stderr.decode(errors="replace")
    if args.score_command:
        score_proc = subprocess.run(
            command_tokens(args.score_command, seed=seed, input=input_path, output=output_path),
            capture_output=True, text=True, timeout=args.scorer_timeout, check=False)
        (case_dir / "scorer_stdout.txt").write_text(score_proc.stdout)
        (case_dir / "scorer_stderr.txt").write_text(score_proc.stderr)
        if score_proc.returncode:
            return Result(seed, None, elapsed, "scorer_crash", score_proc.returncode)
        score_text = score_proc.stdout + "\n" + score_proc.stderr
    score = extract_score(score_text, args.score_regex)
    return Result(seed, score, elapsed, "ok" if score is not None else "score_missing", proc.returncode)


def main() -> None:
    parser = argparse.ArgumentParser(description="AHC multi-case throughput runner")
    parser.add_argument("--solver", required=True, help="command; {seed}, {input}, {output} are expanded")
    parser.add_argument("--input-dir", type=Path, default=Path("in"))
    parser.add_argument("--input-pattern", default="{seed:04d}.txt")
    parser.add_argument("--cases", default="0-99")
    parser.add_argument("--jobs", type=int, default=1)
    parser.add_argument("--timeout", type=float, default=3.0)
    parser.add_argument("--score-command", help="external scorer command")
    parser.add_argument("--scorer-timeout", type=float, default=10.0)
    parser.add_argument("--score-regex", default=r"(?:Score|score)\s*[=:]\s*(-?[0-9]+(?:\.[0-9]+)?)")
    parser.add_argument("--output-dir", type=Path, default=Path("tester/results/latest"))
    parser.add_argument("--csv", type=Path)
    parser.add_argument("--cores", help="comma-separated CPU ids, e.g. 0,2,4 (prefer physical cores)")
    args = parser.parse_args()
    args.input_dir = args.input_dir.resolve(); args.output_dir = args.output_dir.resolve()
    args.output_dir.mkdir(parents=True, exist_ok=True)
    cases = parse_cases(args.cases)
    cores = [int(x) for x in args.cores.split(",")] if args.cores else []
    results: list[Result] = []
    with ThreadPoolExecutor(max_workers=args.jobs) as pool:
        futures = {pool.submit(execute, seed, args, cores[i % len(cores)] if cores else None): seed
                   for i, seed in enumerate(cases)}
        for future in as_completed(futures):
            result = future.result(); results.append(result)
            print(f"{result.case:04d} {result.status:13s} score={result.score} time={result.time:.3f}s", flush=True)
    results.sort(key=lambda x: x.case)
    csv_path = (args.csv or args.output_dir / "results.csv").resolve()
    csv_path.parent.mkdir(parents=True, exist_ok=True)
    with csv_path.open("w", newline="") as f:
        writer = csv.DictWriter(f, fieldnames=Result.__annotations__.keys()); writer.writeheader()
        writer.writerows(asdict(result) for result in results)
    ok = [r for r in results if r.status == "ok"]
    print(f"completed={len(ok)}/{len(results)} csv={csv_path}")


if __name__ == "__main__":
    main()
