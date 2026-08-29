#!/usr/bin/env bash
set -euo pipefail

here=$(cd "$(dirname "$0")" && pwd)
if [[ -x "$here/../../environment/compile" ]]; then
  toolkit=$(cd "$here/../.." && pwd)
else
  toolkit=$(cd "$here/../../heuristic-toolkit" && pwd)
fi

mode=${1:-release}
if [[ "$mode" != release && "$mode" != debug && "$mode" != profile ]]; then
  echo "usage: ./build.sh [release|debug|profile]" >&2
  exit 2
fi

output="$here/solver-$mode"
"$toolkit/environment/compile" "$mode" "$here/main.cpp" "$output"
printf '%s\n' "$mode" > "$here/.last-build-mode"
echo "built: $output"
if [[ "$mode" == debug ]]; then
  echo "note: debugはASan/UBSan/_GLIBCXX_DEBUG付き。正しさ確認用で、速度測定には使わない。"
else
  echo "note: 実行時間を比べるときはreleaseを使う。"
fi
