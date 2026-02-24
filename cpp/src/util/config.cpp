#include <zetaforge/config.hpp>

#include <stdexcept>

#include <yaml-cpp/yaml.h>

namespace zf {

namespace {
RiskConfig parse_risk(const YAML::Node& n) {
  RiskConfig r;
  if (!n) return r;
  r.max_orders_per_sec = n["max_orders_per_sec"].as<double>(r.max_orders_per_sec);
  r.max_notional = n["max_notional"].as<std::uint64_t>(r.max_notional);
  r.max_position = n["max_position"].as<std::int64_t>(r.max_position);
  r.price_band_bps = n["price_band_bps"].as<std::uint32_t>(r.price_band_bps);
  r.kill_switch = n["kill_switch"].as<bool>(r.kill_switch);
  return r;
}

LatencyModel parse_latency(const YAML::Node& n) {
  LatencyModel m;
  if (!n) return m;
  m.base_ns = n["base_ns"].as<std::uint64_t>(m.base_ns);
  m.jitter_ns = n["jitter_ns"].as<std::uint64_t>(m.jitter_ns);
  return m;
}
}

ReplayConfig load_replay_config_yaml(const std::string& path) {
  auto root = YAML::LoadFile(path);
  ReplayConfig c;
  c.input_jsonl = root["input_jsonl"].as<std::string>("");
  c.seed = root["seed"].as<std::uint64_t>(c.seed);
  c.speed = root["speed"].as<double>(c.speed);
  if (c.input_jsonl.empty()) throw std::runtime_error("input_jsonl missing in " + path);
  return c;
}

TradeConfig load_trade_config_yaml(const std::string& path) {
  auto root = YAML::LoadFile(path);
  TradeConfig c;
  c.input_jsonl = root["input_jsonl"].as<std::string>("");
  c.seed = root["seed"].as<std::uint64_t>(c.seed);
  c.order_qty = root["order_qty"].as<std::int64_t>(c.order_qty);
  c.edge_ticks = root["edge_ticks"].as<std::int64_t>(c.edge_ticks);
  c.risk = parse_risk(root["risk"]);
  c.venue_latency = parse_latency(root["venue_latency"]);
  if (c.input_jsonl.empty()) throw std::runtime_error("input_jsonl missing in " + path);
  return c;
}

BenchConfig load_bench_config_yaml(const std::string& path) {
  auto root = YAML::LoadFile(path);
  BenchConfig c;
  c.input_jsonl = root["input_jsonl"].as<std::string>("");
  c.seed = root["seed"].as<std::uint64_t>(c.seed);
  c.warmup_events = root["warmup_events"].as<std::uint64_t>(c.warmup_events);
  c.measure_events = root["measure_events"].as<std::uint64_t>(c.measure_events);
  if (c.input_jsonl.empty()) throw std::runtime_error("input_jsonl missing in " + path);
  return c;
}

} // namespace zf
