"""
Run C++ benchmarks and generate plots.

Usage (from repo root):
  cd python
  python -m orchestration.run_bench --build-dir ..\\build --config ..\\configs\\bench.yaml --out-dir ..\\out
"""

from __future__ import annotations

import argparse
import subprocess
from pathlib import Path

import pandas as pd

from analysis.plot_bench import plot_bench


def run_cmd(cmd: list[str], *, cwd: Path | None = None) -> None:
    print("$", " ".join(cmd))
    if cwd is not None:
        print("  (cwd =", str(cwd), ")")
    subprocess.check_call(cmd, cwd=str(cwd) if cwd is not None else None)


def resolve_from_cwd(p: str) -> Path:
    """Resolve a path exactly as written (relative to current working directory)."""
    return Path(p).expanduser().resolve()


def main() -> None:
    ap = argparse.ArgumentParser()
    ap.add_argument("--build-dir", required=True)
    ap.add_argument("--config", required=True)
    ap.add_argument("--out-dir", required=True)
    args = ap.parse_args()

    build_dir = resolve_from_cwd(args.build_dir)
    out_dir = resolve_from_cwd(args.out_dir)
    config_path = resolve_from_cwd(args.config)

    out_dir.mkdir(parents=True, exist_ok=True)

    # repo_root/python/orchestration/run_bench.py -> parents[2] == repo_root
    repo_root = Path(__file__).resolve().parents[2]

    # Handle Windows .exe if needed
    exe = build_dir / "Zetaforge_bench"
    exe_exe = exe.with_suffix(".exe")
    if exe_exe.exists():
        exe = exe_exe

    if not exe.exists():
        raise FileNotFoundError(f"Benchmark executable not found: {exe}")

    csv_path = out_dir / "bench.csv"

    print("python cwd :", Path.cwd())
    print("repo_root  :", repo_root)
    print("build_dir  :", build_dir)
    print("config     :", config_path)
    print("out_dir    :", out_dir)
    print("csv_path   :", csv_path)

    # IMPORTANT: run the C++ binary from repo_root so 'data/...' resolves correctly
    run_cmd(
        [str(exe), "--config", str(config_path), "--out", str(csv_path)],
        cwd=repo_root,
    )

    df = pd.read_csv(csv_path)
    print(df)

    fig_path = out_dir / "bench_latency.png"
    plot_bench(df, fig_path)
    print("wrote", fig_path)


if __name__ == "__main__":
    main()