#!/usr/bin/env bash
set -euo pipefail

here=$(cd "$(dirname "$0")" && pwd)
input=${1:?usage: ./run.sh in/0000.txt [out/0000.txt]}
output=${2:-"$here/out/$(basename "$input")"}
mkdir -p "$(dirname "$output")"
"$here/solver" < "$input" > "$output"
echo "output: $output"
