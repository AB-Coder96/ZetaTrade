#include <zetaforge/risk/risk_gate.hpp>

#include <cmath>

namespace zf {

RiskGate::RiskGate(RiskConfig cfg) : cfg_(cfg) {
  capacity_ = cfg_.max_orders_per_sec; // allow 1s burst
  tokens_ = capacity_;
  refill_per_ns_ = cfg_.max_orders_per_sec / 1e9;
}

void RiskGate::reset() {
  tokens_ = capacity_;
  last_ts_ = 0;
  position_ = 0;
}

void RiskGate::set_reference_mid(std::optional<Price> mid) { ref_mid_ = mid; }

RiskDecision RiskGate::check_new(Side side, Price price, Quantity qty, TimestampNs ts_ns) {
  (void)side;
  RiskDecision d;

  if (cfg_.kill_switch) {
    d.ok = false;
    d.reason = "kill_switch";
    return d;
  }

  if (qty <= 0 || price <= 0) {
    d.ok = false;
    d.reason = "invalid_qty_or_price";
    return d;
  }

  // Token bucket refill
  if (last_ts_ != 0 && ts_ns > last_ts_) {
    const double dt = static_cast<double>(ts_ns - last_ts_);
    tokens_ = std::min(capacity_, tokens_ + dt * refill_per_ns_);
  }
  last_ts_ = ts_ns;

  if (tokens_ < 1.0) {
    d.ok = false;
    d.reason = "rate_limit";
    return d;
  }
  tokens_ -= 1.0;

  // Notional limit
  const long double notional = static_cast<long double>(price) * static_cast<long double>(qty);
  if (notional > static_cast<long double>(cfg_.max_notional)) {
    d.ok = false;
    d.reason = "max_notional";
    return d;
  }

  // Price bands
  if (ref_mid_.has_value()) {
    const long double mid = static_cast<long double>(*ref_mid_);
    const long double band = mid * static_cast<long double>(cfg_.price_band_bps) / 10'000.0L;
    const long double lo = mid - band;
    const long double hi = mid + band;
    if (static_cast<long double>(price) < lo || static_cast<long double>(price) > hi) {
      d.ok = false;
      d.reason = "price_band";
      return d;
    }
  }

  // Position limit is enforced on fills; allow order, but you could block here too.
  d.ok = true;
  return d;
}

void RiskGate::on_fill(Side side, Price /*price*/, Quantity qty) {
  // Long position is positive.
  position_ += (side == Side::Buy) ? qty : -qty;
  if (std::llabs(position_) > cfg_.max_position) {
    cfg_.kill_switch = true; // latch
  }
}

} // namespace zf
