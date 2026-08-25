#!/usr/bin/env bash
set -euo pipefail
root=$(cd "$(dirname "$0")/.." && pwd)
"$root/environment/compile" debug "$root/tests/test_library.cpp" /tmp/ht_test
/tmp/ht_test
for src in "$root"/templates/*.cpp; do
  "$root/environment/compile" release "$src" "/tmp/$(basename "${src%.cpp}")"
done
python3 -m unittest discover -s "$root/tests" -p 'test_*.py'
