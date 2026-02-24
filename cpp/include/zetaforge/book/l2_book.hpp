#pragma once

#include <cstdint>
#include <map>
#include <optional>

#include <zetaforge/types.hpp>

namespace zf {

// Simple aggregated L2 book (price -> qty). Intended for deterministic replay & validation.
class L2Book {
 public:
  void apply_delta(Side side, Price price, Quantity qty);

  std::optional<Price> best_bid() const;
  std::optional<Price> best_ask() const;

  Quantity level_qty(Side side, Price price) const;

  // Basic sanity checks; returns false if invariants are violated.
  bool validate() const;

 private:
  struct Desc {
    bool operator()(Price a, Price b) const { return a > b; }
  };

  std::map<Price, Quantity, Desc> bids_;
  std::map<Price, Quantity> asks_;
};

} // namespace zf
