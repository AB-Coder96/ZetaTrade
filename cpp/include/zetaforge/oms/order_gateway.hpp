#pragma once

#include <cstdint>
#include <unordered_map>

#include <zetaforge/risk/risk_gate.hpp>
#include <zetaforge/sim/exchange_sim.hpp>
#include <zetaforge/types.hpp>

namespace zf {

// OMS / order-gateway state machine states (simplified).
enum class OrderState : std::uint8_t {
  Sent,
  Acked,
  PartiallyFilled,
  Filled,
  Canceled,
  Rejected,
};

struct OrderInfo {
  OrderId id{0};
  Side side{Side::Buy};
  Price price{0};
  Quantity qty{0};
  Quantity cum_filled{0};
  OrderState state{OrderState::Sent};
};

// Minimal order gateway:
// - allocates client order IDs
// - applies RiskGate checks
// - forwards to ExchangeSim
// - updates internal order states based on ExecReport
class OrderGateway {
 public:
  OrderGateway() = default;

  ExecReport send_limit(Side side, Price price, Quantity qty, TimestampNs ts_ns,
                        RiskGate& risk, ExchangeSim& venue);
  ExecReport send_cancel(OrderId id, TimestampNs ts_ns, ExchangeSim& venue);

  const OrderInfo* get(OrderId id) const;

 private:
  OrderId next_id();
  void apply_exec(const ExecReport& rep);

  OrderId next_{1};
  std::unordered_map<OrderId, OrderInfo> orders_;
};

} // namespace zf