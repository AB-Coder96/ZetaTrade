#include <catch2/catch_test_macros.hpp>

#include <zetaforge/risk/risk_gate.hpp>

TEST_CASE("RiskGate blocks price bands and can trip kill switch on position") {
  zf::RiskConfig cfg;
  cfg.max_orders_per_sec = 1e9;
  cfg.max_position = 10;
  cfg.price_band_bps = 100; // 1%

  zf::RiskGate risk(cfg);
  risk.set_reference_mid(10000);

  auto ok = risk.check_new(zf::Side::Buy, 10050, 1, 1000);
  REQUIRE(ok.ok);

  auto bad = risk.check_new(zf::Side::Buy, 12000, 1, 2000);
  REQUIRE_FALSE(bad.ok);
  REQUIRE(bad.reason == "price_band");

  // Trip kill switch via fills
  risk.on_fill(zf::Side::Buy, 10000, 11);
  REQUIRE(risk.config().kill_switch);
}
