#!/usr/bin/env bash
set -euo pipefail

root=$(cd "$(dirname "$0")/.." && pwd)
source_file=${1:?usage: tools/make_submission.sh source.cpp [submission.cpp] [additional bundle arguments...]}
output_file=${2:-submission.cpp}
if (($# >= 2)); then shift 2; else shift 1; fi

python3 "$root/tools/bundle.py" "$source_file" -o "$output_file" -I "$root" "$@"

if grep -nE '^\s*#\s*include\s*"' "$output_file"; then
  echo "error: unresolved local include remains in $output_file" >&2
  exit 2
fi

check_binary=$(mktemp /tmp/ht-submission-check.XXXXXX)
trap 'rm -f -- "$check_binary"' EXIT
"$root/environment/compile" release "$output_file" "$check_binary"
echo "submission ready: $output_file"
