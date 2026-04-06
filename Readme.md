# ZetaTrade— Deterministic Market Replay & Execution Lab

ZetaTrade is a production-ready trading-systems portfolio project:

- Market data ingestion + normalization
- Deterministic replay + exchange simulation
- Order lifecycle + risk gates (OMS/order gateway state machine)
- Stage-level latency measurement (p50 / p99 / p99.9)
- Research → production bridge (Python → C++ configs / plugins)

## Roadmap (credible next steps)

- Add an ITCH-style equities replay 
- Improve queue-position model for maker strategies
- Continuous benchmark regression via CI
- Drift monitoring + PnL attribution panels in Django React dashboard
- FPGA API for hybrid executions
- spreads, straddles, strangles, iron condors, and other multi-leg structures
- complicate dataset beyond locked markets
- 100k–1M events (including warm up)
- maker|taker mode implement