#pragma once

#include <cstdint>
#include <optional>
#include <string>

#include <zetaforge/types.hpp>

namespace zf {

struct RiskConfig {
  double max_orders_per_sec{50.0};
  std::uint64_t max_notional{10'000'000}; // in price*qty ticks
  std::int64_t max_position{1'000'000};   // in qty units
  std::uint32_t price_band_bps{500};      // 5% default
  bool kill_switch{false};
};

struct RiskDecision {
  bool ok{true};
  std::string reason;
};

class RiskGate {
 public:
  explicit RiskGate(RiskConfig cfg);

  void reset();
  void set_reference_mid(std::optional<Price> mid);

  RiskDecision check_new(Side side, Price price, Quantity qty, TimestampNs ts_ns);
  void on_fill(Side side, Price price, Quantity qty);

  const RiskConfig& config() const { return cfg_; }
  std::int64_t position() const { return position_; }

 private:
  RiskConfig cfg_;
  std::optional<Price> ref_mid_;

  // Token bucket
  double tokens_{0.0};
  double capacity_{0.0};
  double refill_per_ns_{0.0};
  TimestampNs last_ts_{0};

  std::int64_t position_{0};
};

} // namespace zf
