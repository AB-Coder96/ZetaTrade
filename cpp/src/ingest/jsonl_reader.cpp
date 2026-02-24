#include <zetaforge/ingest/jsonl_reader.hpp>

#include <fstream>
#include <stdexcept>

#include <nlohmann/json.hpp>

namespace zf {

namespace {
EventType parse_type(const std::string& s) {
  if (s == "trade") return EventType::Trade;
  if (s == "book_delta") return EventType::BookDelta;
  return EventType::Unknown;
}

Side parse_side(const std::string& s) {
  if (s == "bid" || s == "buy" || s == "B") return Side::Buy;
  if (s == "ask" || s == "sell" || s == "S") return Side::Sell;
  return Side::Buy;
}
}

std::vector<MarketEvent> read_jsonl_events(const std::string& path) {
  std::ifstream in(path);
  if (!in) throw std::runtime_error("Failed to open input: " + path);

  std::vector<MarketEvent> out;
  out.reserve(1 << 16);

  std::string line;
  while (std::getline(in, line)) {
    if (line.empty()) continue;
    auto j = nlohmann::json::parse(line);

    MarketEvent e;
    e.ts_ns = j.value("ts_ns", 0ULL);
    e.type = parse_type(j.value("type", ""));

    if (e.type == EventType::BookDelta) {
      e.side = parse_side(j.value("side", "bid"));
      e.price = j.value("price", 0LL);
      e.qty = j.value("qty", 0LL);
    } else if (e.type == EventType::Trade) {
      e.trade_px = j.value("price", 0LL);
      e.trade_qty = j.value("qty", 0LL);
    }

    out.push_back(e);
  }
  return out;
}

} // namespace zf
