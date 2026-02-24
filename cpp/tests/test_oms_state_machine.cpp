#include <catch2/catch_test_macros.hpp>

#include <zetaforge/oms/order_gateway.hpp>

TEST_CASE("OMS transitions on ack/fill") {
  zf::RiskConfig rc;
  rc.max_orders_per_sec = 1e9; // effectively off for test
  zf::RiskGate risk(rc);

  zf::ExchangeSim venue(42);
  zf::OrderGateway oms;

  // Resting sell
  auto rep_s = oms.send_limit(zf::Side::Sell, 101, 10, 1'000, risk, venue);
  REQUIRE(rep_s.status == zf::ExecStatus::Ack);
  auto s = oms.get(rep_s.order_id);
  REQUIRE(s);
  REQUIRE(s->state == zf::OrderState::Acked);

  // Buy crosses -> fill
  auto rep_b = oms.send_limit(zf::Side::Buy, 105, 10, 2'000, risk, venue);
  REQUIRE(rep_b.status == zf::ExecStatus::Fill);
  auto b = oms.get(rep_b.order_id);
  REQUIRE(b);
  REQUIRE(b->state == zf::OrderState::Filled);

  // Maker should have been filled by that trade (but we don't update maker state in this simplified OMS)
  // In a production OMS, maker reports would be routed back separately.
}
