-- Minimal schema for runs + metrics.
-- If you integrate with ZetaPulse / MarketOps, evolve this into proper dimensional tables.

CREATE TABLE IF NOT EXISTS bench_runs (
  run_id       BIGSERIAL PRIMARY KEY,
  created_at   TIMESTAMPTZ NOT NULL DEFAULT NOW(),
  scenario     TEXT NOT NULL,
  events       BIGINT NOT NULL,
  seconds      DOUBLE PRECISION NOT NULL,
  throughput_eps DOUBLE PRECISION NOT NULL,
  book_p50_ns  BIGINT NOT NULL,
  book_p99_ns  BIGINT NOT NULL,
  book_p999_ns BIGINT NOT NULL
);
