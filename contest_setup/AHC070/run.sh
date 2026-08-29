#!/usr/bin/env bash
set -euo pipefail

here=$(cd "$(dirname "$0")" && pwd)
input=${1:?usage: ./run.sh in/0000.txt [out/0000.txt]}
output=${2:-"$here/out/$(basename "$input")"}
mode=${HT_MODE:-release}
timeout_sec=${HT_TIMEOUT:-5}
solver="$here/solver-$mode"
stderr_log="$output.stderr"

if [[ ! -x "$solver" ]]; then
  echo "[run] $solver が無いため、先に $mode buildを行う。"
  "$here/build.sh" "$mode"
fi

mkdir -p "$(dirname "$output")"
echo "[run] input   : $input"
echo "[run] output  : $output"
echo "[run] stderr  : $stderr_log（画面にも同時表示）"
echo "[run] mode    : $mode"
echo "[run] timeout : ${timeout_sec}s（変更: HT_TIMEOUT=30 ./run.sh ...）"
echo "[run] action  : solverを1回起動し、stdinをinputから、stdoutをoutputへ接続する。"
echo "[run] note    : 以降の待ち時間はsolver内部の探索。run.sh自身が複数caseを回しているわけではない。"
if [[ "$mode" == debug ]]; then
  echo "[run] WARNING : debugはsanitizer/_GLIBCXX_DEBUG付き。releaseより大幅に遅く、時間比較には不向き。"
fi

start=$(date +%s)
set +e
/usr/bin/time -f '[time] wall=%e sec, user=%U, sys=%S, maxrss=%M KB, exit=%x' \
  timeout --signal=TERM --kill-after=1s "${timeout_sec}s" \
  "$solver" < "$input" > "$output" 2> >(tee "$stderr_log" >&2) &
pid=$!
while kill -0 "$pid" 2>/dev/null; do
  sleep 1
  if kill -0 "$pid" 2>/dev/null; then
    now=$(date +%s)
    echo "[run] running... $((now - start))s elapsed"
  fi
done
wait "$pid"
status=$?
set -e

if [[ "$status" == 124 ]]; then
  echo "[run] TIMEOUT: ${timeout_sec}sを超えた。探索上限、build mode、無限loopを確認する。" >&2
  exit 124
fi
if [[ "$status" != 0 ]]; then
  echo "[run] FAILED: exit=$status。詳細は $stderr_log" >&2
  exit "$status"
fi
echo "[run] OK: output=$output"
