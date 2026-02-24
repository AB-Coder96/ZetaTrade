#include <cstdint>
#include <iostream>

#include <CLI/CLI.hpp>

#include <zetaforge/book/l2_book.hpp>
#include <zetaforge/config.hpp>
#include <zetaforge/ingest/jsonl_reader.hpp>
#include <zetaforge/oms/order_gateway.hpp>
#include <zetaforge/risk/risk_gate.hpp>
#include <zetaforge/sim/exchange_sim.hpp>
#include <zetaforge/util/logging.hpp>

namespace {
std::optional<zf::Price> mid_from(const std::optional<zf::Price>& bb, const std::optional<zf::Price>& ba) {
  if (!bb || !ba) return std::nullopt;
  return (*bb + *ba) / 2;
}
}

int main(int argc, char** argv) {
  CLI::App app{"Zetaforge_trade — replay + demo strategy + risk + exchange sim"};

  std::string config_path;
  std::string log_level = "info";
  std::uint64_t seed_override = 0;

  app.add_option("--config", config_path, "YAML config")->required();
  app.add_option("--log-level", log_level, "Log level (trace/debug/info/warn/error)");
  app.add_option("--seed", seed_override, "Override seed from config (0 = use config)");

  CLI11_PARSE(app, argc, argv);

  zf::init_logging(log_level);

  auto cfg = zf::load_trade_config_yaml(config_path);
  if (seed_override != 0) cfg.seed = seed_override;

  auto events = zf::read_jsonl_events(cfg.input_jsonl);

  zf::L2Book market_book;
  zf::RiskGate risk(cfg.risk);
  zf::ExchangeSim venue(cfg.seed, cfg.venue_latency);
  zf::OrderGateway oms;

  std::uint64_t i = 0;
  std::uint64_t sent = 0;
  std::uint64_t fills = 0;

  for (const auto& e : events) {
    if (e.type == zf::EventType::BookDelta) {
      market_book.apply_delta(e.side, e.price, e.qty);
      if (!market_book.validate()) {
        ZF_LOG_WARN("market_book crossed/invalid at event {}", i);
      }

      auto bb = market_book.best_bid();
      auto ba = market_book.best_ask();
      risk.set_reference_mid(mid_from(bb, ba));

      // Naive demo strategy: every N updates, place one bid/ask inside the spread.
      if (bb && ba && (i % 50 == 0)) {
        const zf::Price spread = *ba - *bb;
        if (spread > (2 * cfg.edge_ticks + 1)) {
          const zf::Price bid_px = *bb + cfg.edge_ticks;
          const zf::Price ask_px = *ba - cfg.edge_ticks;

          auto rep_b = oms.send_limit(zf::Side::Buy, bid_px, cfg.order_qty, e.ts_ns, risk, venue);
          sent++;
          fills += rep_b.fills.size();

          auto rep_s = oms.send_limit(zf::Side::Sell, ask_px, cfg.order_qty, e.ts_ns, risk, venue);
          sent++;
          fills += rep_s.fills.size();
        }
      }
    }

    if (risk.config().kill_switch) {
      ZF_LOG_ERROR("Kill-switch tripped. position={} at event {}", risk.position(), i);
      break;
    }

    ++i;
  }

  std::cout << "events=" << i << "\n";
  std::cout << "orders_sent=" << sent << "\n";
  std::cout << "fill_events=" << fills << "\n";
  std::cout << "final_position=" << risk.position() << "\n";
  std::cout << "venue_best_bid=" << (venue.best_bid() ? std::to_string(*venue.best_bid()) : "NA") << "\n";
  std::cout << "venue_best_ask=" << (venue.best_ask() ? std::to_string(*venue.best_ask()) : "NA") << "\n";

  return 0;
}
