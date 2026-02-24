#pragma once

#include <cstdint>
#include <deque>
#include <map>
#include <optional>
#include <random>
#include <vector>

#include <zetaforge/types.hpp>

namespace zf {

enum class ExecStatus : std::uint8_t { Ack, PartialFill, Fill, Canceled, Rejected };

struct Fill {
  OrderId maker_id{0};
  OrderId taker_id{0};
  Price price{0};
  Quantity qty{0};
  TimestampNs ts_ns{0};
};

struct ExecReport {
  OrderId order_id{0};
  ExecStatus status{ExecStatus::Ack};
  Quantity leaves_qty{0};
  Quantity cum_filled{0};
  std::vector<Fill> fills;
  TimestampNs venue_ts_ns{0};
};

struct LatencyModel {
  std::uint64_t base_ns{50'000};   // 50us
  std::uint64_t jitter_ns{10'000}; // +/- 10us
};

class ExchangeSim {
 public:
  explicit ExchangeSim(std::uint64_t seed, LatencyModel model = {});

  ExecReport submit_limit(OrderId id, Side side, Price price, Quantity qty, TimestampNs client_ts_ns);
  ExecReport cancel(OrderId id, TimestampNs client_ts_ns);

  // Best price of internal book (resting orders)
  std::optional<Price> best_bid() const;
  std::optional<Price> best_ask() const;

 private:
  struct Order {
    OrderId id{0};
    Side side{Side::Buy};
    Price price{0};
    Quantity qty{0};
    Quantity filled{0};
    TimestampNs ts_ns{0};
  };

  struct Desc {
    bool operator()(Price a, Price b) const { return a > b; }
  };

  using Level = std::deque<Order>;

  std::map<Price, Level, Desc> bids_;
  std::map<Price, Level> asks_;

  LatencyModel latency_;
  std::mt19937_64 rng_;

  TimestampNs venue_time(TimestampNs client_ts_ns);

  ExecReport match_against_book(Order taker, TimestampNs venue_ts_ns);
  bool erase_order(OrderId id);
};

} // namespace zf
