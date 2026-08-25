#!/usr/bin/env bash
set -euo pipefail
root=$(cd "$(dirname "$0")/.." && pwd)
mkdir -p "$root/benchmark/results"
"$root/environment/compile" release "$root/benchmark/micro/micro.cpp" /tmp/ht_micro
stamp=$(date -u +%Y%m%dT%H%M%SZ)
out="$root/benchmark/results/micro-$stamp.csv"
/tmp/ht_micro > "$out"
{
  uname -a
  g++ --version | head -1
  lscpu | grep -E 'Model name|Socket|Core|Thread'
} > "$root/benchmark/results/environment-$stamp.txt"
echo "$out"
