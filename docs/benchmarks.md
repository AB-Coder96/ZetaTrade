# Benchmarks

## What is measured?

The built-in `Zetaforge_bench` focuses on microbenchmarks that are common in interviews:

- Replay → L2 book update latency

Next steps you can add:

- Sim → OMS roundtrip latency (including risk gate)
- End-to-end replay→fill latency with venue latency injection

## How to run

```bash
./Zetaforge_bench --config ../configs/bench.yaml --out ../out/bench.csv
```

## Reproducibility checklist

- Pin cores / set CPU affinity
- Warm up before measuring
- Fixed input dataset + seed
- Release build
- Record machine model + kernel version
