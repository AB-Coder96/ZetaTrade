#pragma once

#include <cstdint>
#include <string>

namespace zf {

using OrderId = std::uint64_t;
using Quantity = std::int64_t; // integer lots (e.g., satoshis, contracts)
using Price = std::int64_t;    // integer ticks (avoid floating point on hot path)
using TimestampNs = std::uint64_t;

enum class Side : std::uint8_t { Buy = 0, Sell = 1 };

enum class EventType : std::uint8_t {
  Trade,
  BookDelta,
  OrderNew,
  OrderCancel,
  OrderReplace,
  Unknown
};

struct MarketEvent {
  TimestampNs ts_ns{0};
  EventType type{EventType::Unknown};

  // For BookDelta
  Side side{Side::Buy};
  Price price{0};
  Quantity qty{0};

  // For Trade
  Price trade_px{0};
  Quantity trade_qty{0};
};

inline const char* to_string(Side s) {
  return s == Side::Buy ? "BUY" : "SELL";
}

} // namespace zf
