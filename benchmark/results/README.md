# Benchmark results

Run `benchmark/run_micro.sh`; timestamped CSV and environment metadata are written here.
Do not transfer results between machines. Check the hot search loop as well: a 2x faster
primitive is irrelevant if it is 1% of runtime. Compare `quality/iteration`,
`iterations/second`, and final paired-seed score.
