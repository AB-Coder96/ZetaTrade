# Design notes

## Determinism

Zetaforge aims to be deterministic: same inputs + same seed + same config ⇒ same outputs.

Practices used in this repo:

- Integer ticks for prices/quantities on the engine hot-path
- Seeded RNG for any stochastic components (e.g., venue latency injection)
- No dependence on wall-clock time for simulation decisions

## Hot-path discipline

The core loop should keep control flow predictable and avoid heap churn.

This scaffold keeps the hot-path small, but is intentionally conservative:

- Logging is compile-time gated via `ZF_ENABLE_LOGGING`
- Latency measurement uses a tiny log2 histogram

## Test strategy

- Unit tests: order book invariants, OMS state transitions, risk checks
- Golden tests (next step): replay a fixed dataset and compare a stable checksum of state
