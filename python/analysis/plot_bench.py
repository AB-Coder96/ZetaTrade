"""Plot benchmark CSV outputs."""

from __future__ import annotations

from pathlib import Path

import matplotlib.pyplot as plt
import pandas as pd


def plot_bench(df: pd.DataFrame, out_path: str | Path) -> None:
    # One-row CSV by default; keep robust for multiple runs.
    out_path = Path(out_path)

    x = range(len(df))

    plt.figure()
    plt.plot(x, df["book_p50_ns"], label="p50", marker="o")
    plt.plot(x, df["book_p99_ns"], label="p99", marker="o")
    plt.plot(x, df["book_p999_ns"], label="p99.9", marker="o")
    plt.xlabel("run")
    plt.ylabel("latency (ns)")
    plt.yscale("log")
    plt.title("Replay → Book Update Latency")
    plt.legend()
    plt.tight_layout()

    out_path.parent.mkdir(parents=True, exist_ok=True)
    plt.savefig(out_path)
    plt.close()