# Benchmark discipline

## Throughput mode

Use `tester/run.py --jobs N` for hundreds/thousands of seeds. Choose physical cores
first. This measures score efficiently; individual wall times are contaminated by
SMT, cache, memory bandwidth, scheduler and thermal contention.

## Benchmark mode

Use `tester/benchmark.py` with one process, one representative input, warmups and a
fixed CPU. Report median, p25/p75, min/max and coefficient of variation. Repeat after
the machine reaches a stable temperature. WSL and Docker add layers that may differ
from the judge, so keep a safety margin and calibrate on actual submissions when rules
allow.

On Linux inspect topology with `lscpu -e=CPU,CORE,SOCKET,NODE`. On this machine the
initial audit found 6 physical / 12 logical Ryzen 5 3600X CPUs, so a reasonable
throughput starting point is six workers on distinct CORE values.

Other noise sources: turbo, thermal throttling, power plan, background programs,
antivirus scanning newly-created executables/files, process startup and filesystem
I/O. Log environment metadata with every micro benchmark.
