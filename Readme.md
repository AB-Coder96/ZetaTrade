# Zetaforge — Deterministic Market Replay & Execution Lab

Zetaforge is a production-ready trading-systems portfolio project:

- Market data ingestion + normalization
- Deterministic replay + exchange simulation
- Order lifecycle + risk gates (OMS/order gateway state machine)
- Stage-level latency measurement (p50 / p99 / p99.9)
- Research → production bridge (Python → C++ configs / plugins)

## Stack

- **C++20** (engine hot path)
- **CMake** (build)
- **Catch2** (unit tests)
- **CLI11** (CLI args)
- **fmt + spdlog** (logging; keep logging off hot-path)
- **nlohmann/json + yaml-cpp** (JSONL datasets, YAML configs)
- **Python 3.10+** (analysis + plotting + run orchestration)
  - pandas, matplotlib (optional numpy)
- **Data/storage**
  - Local CSV/Parquet outputs initially
  - Optional Postgres/TimescaleDB for runs/metrics (docker-compose)
- **Deploy**
  - Docker
  - GitHub Actions

## Architecture (high level)

```
[Live / Recorded Market Data]
          |
          v
   Ingest + Normalize  --->  Persistent event log (optional)
          |
          v
   Deterministic Replay Engine
          |
          v
  Exchange Simulator (matching, cancels, partial fills, latency injection)
          |
          v
 Strategy Plugin / Zeta Module (C++ runtime, Python research configs)
          |
          v
 Risk Gate (limits, price bands, throttle, kill-switch)
          |
          v
 Order Gateway (state machine)  --->  Metrics/Tracing/Benchmarks
```

## Quickstart

### Requirements

- Linux recommended (macOS works for most components)
- C++20 compiler (clang/gcc)
- Python 3.10+
- Optional: Docker + docker-compose for the full stack

### Build (C++)
run this manually

```bash
mkdir -p build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . -j
ctest --test-dir .
```

or on windows:

 run the runtest.bat folder

### Run a deterministic replay

```bash
./Zetaforge_replay --input ../data/sample_day.jsonl --seed 42 --speed 10
```

### Run simulator + execution loop

```bash
./Zetaforge_trade --config ../configs/demo_strategy.yaml --seed 42
```

### Export benchmark results

```bash
./Zetaforge_bench --config ../configs/bench.yaml --out ../out/bench.csv
```
## Determinism & Bench Methodology

- Fixed dataset + seed: every benchmark run should specify --input <file> and --seed <N>

- Warmup: run a warmup period (e.g., 10s) before sampling

- CPU pinning / affinity: pin the hot thread to a single core when measuring tails (p99/p99.9)

- Release flags: build in Release (-O3 -DNDEBUG) and record compiler version

- Record environment: CPU model + OS/kernel version per run (store in out/latest/meta.json or similar)

- Logging off hot-path: spdlog should be disabled or minimal on the critical path during benchmarks


## Benchmarks (autoupdated at runs)

| Scenario | Throughput (msgs/sec) | p50 | p99 | p99.9 | Notes |
|---|---:|---:|---:|---:|---|
| Replay → Book update | TBD | TBD | TBD | TBD | pinned core, warmup 10s |
| Sim → OMS roundtrip | TBD | TBD | TBD | TBD | includes risk + state machine |
| End-to-end (replay→fill) | TBD | TBD | TBD | TBD | includes simulator latency model |

### Methodology checklist

- CPU pinning / affinity
- warm-up period
- fixed input dataset + seed
- release build flags
- record machine + kernel version

## Repo layout

```
Zetaforge/
  cpp/
    include/zetaforge/
    src/
      ingest/ book/ sim/ oms/ risk/ bench/ util/
    apps/            # Zetaforge_* executables
    tests/
  python/
    orchestration/   # run orchestration
    analysis/        # plotting + analysis
  configs/
  data/              # small sample datasets only
  docs/
  out/
  docker/
```

## Roadmap (credible next steps)

- Add an ITCH-style equities replay (or futures incremental feed) after crypto baseline
- Improve queue-position model for maker strategies
- Add kernel-bypass ingest experiment (AF_XDP) behind a feature flag
- Continuous benchmark regression via CI
- Drift monitoring + PnL attribution panels in MarketOps dashboard
