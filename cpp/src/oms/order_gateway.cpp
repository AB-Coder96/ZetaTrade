#include <zetaforge/oms/order_gateway.hpp>

namespace zf {

OrderId OrderGateway::next_id() { return next_++; }

const OrderInfo* OrderGateway::get(OrderId id) const {
  auto it = orders_.find(id);
  return it == orders_.end() ? nullptr : &it->second;
}

ExecReport OrderGateway::send_limit(Side side, Price price, Quantity qty, TimestampNs ts_ns,
                                   RiskGate& risk, ExchangeSim& venue) {
  const auto id = next_id();
  orders_.emplace(id, OrderInfo{id, side, price, qty, 0, OrderState::Sent});

  auto dec = risk.check_new(side, price, qty, ts_ns);
  if (!dec.ok) {
    ExecReport rep;
    rep.order_id = id;
    rep.status = ExecStatus::Rejected;
    rep.leaves_qty = qty;
    rep.cum_filled = 0;
    rep.venue_ts_ns = ts_ns;
    apply_exec(rep);
    return rep;
  }

  auto rep = venue.submit_limit(id, side, price, qty, ts_ns);
  // Update risk on fills
  for (const auto& f : rep.fills) {
    // taker is our order
    risk.on_fill(side, f.price, f.qty);
  }
  apply_exec(rep);
  return rep;
}

ExecReport OrderGateway::send_cancel(OrderId id, TimestampNs ts_ns, ExchangeSim& venue) {
  auto rep = venue.cancel(id, ts_ns);
  apply_exec(rep);
  return rep;
}

void OrderGateway::apply_exec(const ExecReport& rep) {
  auto it = orders_.find(rep.order_id);
  if (it == orders_.end()) return;

  auto& o = it->second;
  o.cum_filled += rep.cum_filled;

  switch (rep.status) {
    case ExecStatus::Ack:
      o.state = OrderState::Acked;
      break;
    case ExecStatus::PartialFill:
      o.state = OrderState::PartiallyFilled;
      break;
    case ExecStatus::Fill:
      o.state = OrderState::Filled;
      break;
    case ExecStatus::Canceled:
      o.state = OrderState::Canceled;
      break;
    case ExecStatus::Rejected:
      o.state = OrderState::Rejected;
      break;
  }
}

} // namespace zf
