# AtCoder-compatible build environment

Verified against AtCoder's 2025-10 language list and the 2026-06-15 C++ update.
The current choice is **C++23 / GCC 15.2.0**. The essential submission flags are
`-std=gnu++23 -O2 -march=native -Wall -Wextra -DONLINE_JUDGE -DATCODER -pthread`.
AtCoder removed `-flto=auto` from this language on 2026-06-15, so it is intentionally absent.

```bash
environment/compile release main.cpp solver
environment/compile debug main.cpp solver-debug
environment/compile profile main.cpp solver-profile
docker build -t ahc-gcc15 -f environment/Dockerfile .
docker run --rm -it -v "$PWD:/work" ahc-gcc15
```

`-march=native` means instruction availability depends on the judge CPU. Do not copy a
binary built locally to the judge; submit source. The Docker image matches compiler and
language mode, not AtCoder's exact CPU, kernel, or complete preinstalled library image.
Official listed versions include ac-library 1.6, Boost 1.88.0, Eigen 3.4.0, GMP 6.3.0,
libtorch 2.8.0, OR-Tools 9.14, range-v3 0.12.0, unordered_dense 4.5.0 and Z3 4.15.2.
The toolkit itself deliberately depends only on the standard library.

Sources are listed in `handbook/data/sources.md`.
