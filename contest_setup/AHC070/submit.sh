#!/usr/bin/env bash
set -euo pipefail

here=$(cd "$(dirname "$0")" && pwd)
if [[ -x "$here/../../tools/make_submission.sh" ]]; then
  toolkit=$(cd "$here/../.." && pwd)
else
  toolkit=$(cd "$here/../../heuristic-toolkit" && pwd)
fi
"$toolkit/tools/make_submission.sh" "$here/main.cpp" "$here/submission.cpp"
