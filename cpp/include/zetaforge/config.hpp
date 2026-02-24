#pragma once

#include <cstdint>
#include <string>

#include <zetaforge/risk/risk_gate.hpp>
#include <zetaforge/sim/exchange_sim.hpp>

namespace zf {

struct ReplayConfig {
  std::string input_jsonl;
  std::uint64_t seed{42};
  double speed{1.0}; // multiplier for simulated clock progression
};

struct TradeConfig {
  std::string input_jsonl;
  std::uint64_t seed{42};

  // naive demo strategy
  std::int64_t order_qty{10};
  std::int64_t edge_ticks{1};

  RiskConfig risk{};
  LatencyModel venue_latency{};
};

struct BenchConfig {
  std::string input_jsonl;
  std::uint64_t seed{42};
  std::uint64_t warmup_events{10'000};
  std::uint64_t measure_events{100'000};
};

ReplayConfig load_replay_config_yaml(const std::string& path);
TradeConfig load_trade_config_yaml(const std::string& path);
BenchConfig load_bench_config_yaml(const std::string& path);

} // namespace zf
