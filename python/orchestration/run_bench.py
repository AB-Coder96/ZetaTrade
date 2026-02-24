"""Run C++ benchmarks and generate plots.

Usage:
  python -m orchestration.run_bench --build-dir ../build --config ../configs/bench.yaml --out-dir ../out

This script is intentionally simple: it shells out to the compiled binary.
"""

from __future__ import annotations

import argparse
import subprocess
from pathlib import Path

import pandas as pd

from analysis.plot_bench import plot_bench


def run_cmd(cmd: list[str]) -> None:
    print("$", " ".join(cmd))
    subprocess.check_call(cmd)


def main() -> None:
    ap = argparse.ArgumentParser()
    ap.add_argument("--build-dir", required=True)
    ap.add_argument("--config", required=True)
    ap.add_argument("--out-dir", required=True)
    args = ap.parse_args()

    build_dir = Path(args.build_dir)
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    csv_path = out_dir / "bench.csv"

    exe = build_dir / "Zetaforge_bench"
    run_cmd([str(exe), "--config", args.config, "--out", str(csv_path)])

    df = pd.read_csv(csv_path)
    print(df)

    fig_path = out_dir / "bench_latency.png"
    plot_bench(df, fig_path)
    print("wrote", fig_path)


if __name__ == "__main__":
    main()
