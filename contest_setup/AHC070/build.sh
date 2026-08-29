#!/usr/bin/env bash
set -euo pipefail

here=$(cd "$(dirname "$0")" && pwd)
if [[ -x "$here/../../environment/compile" ]]; then
  toolkit=$(cd "$here/../.." && pwd)
else
  toolkit=$(cd "$here/../../heuristic-toolkit" && pwd)
fi

mode=${1:-debug}
"$toolkit/environment/compile" "$mode" "$here/main.cpp" "$here/solver"
echo "built: $here/solver ($mode)"
