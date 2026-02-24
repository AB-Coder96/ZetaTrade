"""Export bench.csv into Postgres/TimescaleDB.

Usage:
  python -m analysis.export_to_db --csv ../out/bench.csv

Connection is driven by env vars (see docker-compose):
  ZF_DB_HOST, ZF_DB_PORT, ZF_DB_USER, ZF_DB_PASSWORD, ZF_DB_NAME
"""

from __future__ import annotations

import argparse
import os

import pandas as pd
import psycopg2


def main() -> None:
    ap = argparse.ArgumentParser()
    ap.add_argument("--csv", required=True)
    args = ap.parse_args()

    df = pd.read_csv(args.csv)

    conn = psycopg2.connect(
        host=os.environ.get("ZF_DB_HOST", "localhost"),
        port=int(os.environ.get("ZF_DB_PORT", "5432")),
        user=os.environ.get("ZF_DB_USER", "zetaforge"),
        password=os.environ.get("ZF_DB_PASSWORD", "zetaforge"),
        dbname=os.environ.get("ZF_DB_NAME", "zetaforge"),
    )

    with conn, conn.cursor() as cur:
        for _, row in df.iterrows():
            cur.execute(
                """
                INSERT INTO bench_runs
                  (scenario, events, seconds, throughput_eps, book_p50_ns, book_p99_ns, book_p999_ns)
                VALUES (%s,%s,%s,%s,%s,%s,%s)
                """,
                (
                    row["scenario"],
                    int(row["events"]),
                    float(row["seconds"]),
                    float(row["throughput_eps"]),
                    int(row["book_p50_ns"]),
                    int(row["book_p99_ns"]),
                    int(row["book_p999_ns"]),
                ),
            )

    print("Inserted", len(df), "rows")


if __name__ == "__main__":
    main()
