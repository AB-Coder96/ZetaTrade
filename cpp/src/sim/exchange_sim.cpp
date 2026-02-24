#include <zetaforge/sim/exchange_sim.hpp>

#include <algorithm>
#include <random>

namespace zf {

ExchangeSim::ExchangeSim(std::uint64_t seed, LatencyModel model)
    : latency_(model), rng_(seed) {}

TimestampNs ExchangeSim::venue_time(TimestampNs client_ts_ns) {
  std::uniform_int_distribution<std::uint64_t> dist(0, latency_.jitter_ns * 2);
  const auto jitter = dist(rng_);
  const auto delay = latency_.base_ns + jitter - latency_.jitter_ns;
  return client_ts_ns + delay;
}

std::optional<Price> ExchangeSim::best_bid() const {
  if (bids_.empty()) return std::nullopt;
  // bids_ is usually ordered descending (best bid at begin)
  return bids_.begin()->first;
}

std::optional<Price> ExchangeSim::best_ask() const {
  if (asks_.empty()) return std::nullopt;
  // asks_ is ordered ascending (best ask at begin)
  return asks_.begin()->first;
}

ExecReport ExchangeSim::submit_limit(OrderId id,
                                    Side side,
                                    Price price,
                                    Quantity qty,
                                    TimestampNs client_ts_ns) {
  ExecReport rep;
  rep.order_id = id;
  rep.leaves_qty = qty;

  if (qty <= 0 || price <= 0) {
    rep.status = ExecStatus::Rejected;
    rep.venue_ts_ns = venue_time(client_ts_ns);
    rep.cum_filled = 0;
    rep.leaves_qty = 0;
    return rep;
  }

  const auto vts = venue_time(client_ts_ns);
  rep.venue_ts_ns = vts;

  Order taker{id, side, price, qty, 0, vts};
  rep = match_against_book(taker, vts);

  // If not fully filled, rest it
  if (rep.leaves_qty > 0 && rep.status != ExecStatus::Rejected) {
    Order rest{id, side, price, rep.leaves_qty, 0, vts};
    if (side == Side::Buy) {
      bids_[price].push_back(rest);
    } else {
      asks_[price].push_back(rest);
    }

    // If it rested, ensure status reflects fills
    if (rep.cum_filled == 0) rep.status = ExecStatus::Ack;
    else rep.status = ExecStatus::PartialFill;
  }

  return rep;
}

ExecReport ExchangeSim::cancel(OrderId id, TimestampNs client_ts_ns) {
  ExecReport rep;
  rep.order_id = id;
  rep.venue_ts_ns = venue_time(client_ts_ns);

  const bool ok = erase_order(id);
  rep.status = ok ? ExecStatus::Canceled : ExecStatus::Rejected;
  rep.leaves_qty = 0;
  rep.cum_filled = 0;
  return rep;
}

bool ExchangeSim::erase_order(OrderId id) {
  auto erase_from = [&](auto& book) -> bool {
    for (auto it = book.begin(); it != book.end(); ++it) {
      auto& dq = it->second;
      for (auto oit = dq.begin(); oit != dq.end(); ++oit) {
        if (oit->id == id) {
          dq.erase(oit);
          if (dq.empty()) book.erase(it);
          return true;
        }
      }
    }
    return false;
  };

  if (erase_from(bids_)) return true;
  return erase_from(asks_);
}

ExecReport ExchangeSim::match_against_book(Order taker, TimestampNs venue_ts_ns) {
  ExecReport rep;
  rep.order_id = taker.id;
  rep.venue_ts_ns = venue_ts_ns;
  rep.fills.clear();

  Quantity leaves = taker.qty;
  Quantity cum_filled = 0;

  // price crossing condition vs opposite book
  auto crosses = [&](Price opp_px) -> bool {
    if (taker.side == Side::Buy) {
      // buy crosses asks at prices <= limit
      return opp_px <= taker.price;
    } else {
      // sell crosses bids at prices >= limit
      return opp_px >= taker.price;
    }
  };

  // bids_ and asks_ are different map types (different comparators), so
  // avoid ternary selection. Use a templated lambda.
  auto match_on = [&](auto& opp_book) {
    while (leaves > 0 && !opp_book.empty()) {
      auto level_it = opp_book.begin();
      const Price opp_px = level_it->first;

      if (!crosses(opp_px)) break;

      auto& q = level_it->second;

      while (leaves > 0 && !q.empty()) {
        auto& maker = q.front();

        const Quantity maker_leaves = maker.qty - maker.filled;
        if (maker_leaves <= 0) {
          q.pop_front();
          continue;
        }

        const Quantity fill_qty = std::min(leaves, maker_leaves);

        maker.filled += fill_qty;
        leaves -= fill_qty;
        cum_filled += fill_qty;

        rep.fills.push_back(Fill{maker.id, taker.id, opp_px, fill_qty, venue_ts_ns});

        if (maker.filled >= maker.qty) {
          q.pop_front();
        }
      }

      if (q.empty()) {
        opp_book.erase(level_it);
      }
    }
  };

  if (taker.side == Side::Buy) {
    match_on(asks_);
  } else {
    match_on(bids_);
  }

  rep.cum_filled = cum_filled;
  rep.leaves_qty = leaves;

  if (cum_filled == 0) {
    rep.status = ExecStatus::Ack;
  } else if (leaves == 0) {
    rep.status = ExecStatus::Fill;
  } else {
    rep.status = ExecStatus::PartialFill;
  }

  return rep;
}

} // namespace zf