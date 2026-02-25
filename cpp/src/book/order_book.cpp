#include <zetaforge/book/l2_book.hpp>

namespace zf {

void L2Book::apply_delta(Side side, Price price, Quantity qty) {
  if (side == Side::Buy) {
    if (qty <= 0) {
      bids_.erase(price);
    } else {
      bids_[price] = qty;
    }
  } else {
    if (qty <= 0) {
      asks_.erase(price);
    } else {
      asks_[price] = qty;
    }
  }
}

std::optional<Price> L2Book::best_bid() const {
  if (bids_.empty()) return std::nullopt;
  return bids_.begin()->first;
}

std::optional<Price> L2Book::best_ask() const {
  if (asks_.empty()) return std::nullopt;
  return asks_.begin()->first;
}

Quantity L2Book::level_qty(Side side, Price price) const {
  if (side == Side::Buy) {
    auto it = bids_.find(price);
    return it == bids_.end() ? 0 : it->second;
  }
  auto it = asks_.find(price);
  return it == asks_.end() ? 0 : it->second;
}

bool L2Book::validate() const {
  // No negative quantities.
  for (auto& kv : bids_) {
    if (kv.second < 0) return false;
  }
  for (auto& kv : asks_) {
    if (kv.second < 0) return false;
  }
  // Crossed book check if both sides exist.
  if (!bids_.empty() && !asks_.empty()) {
    const auto bb = bids_.begin()->first;
    const auto ba = asks_.begin()->first;
    // Allow locked markets (bb == ba). Only reject truly crossed books.
    if (bb > ba) return false;
  }
  return true;
}

} // namespace zf
