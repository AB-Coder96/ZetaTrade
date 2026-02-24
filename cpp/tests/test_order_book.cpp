#include <catch2/catch_test_macros.hpp>

#include <zetaforge/book/l2_book.hpp>

TEST_CASE("L2Book best bid/ask and validation") {
  zf::L2Book b;
  REQUIRE(b.validate());

  b.apply_delta(zf::Side::Buy, 100, 10);
  b.apply_delta(zf::Side::Buy, 99, 20);
  b.apply_delta(zf::Side::Sell, 101, 5);

  REQUIRE(b.best_bid().has_value());
  REQUIRE(b.best_ask().has_value());
  REQUIRE(*b.best_bid() == 100);
  REQUIRE(*b.best_ask() == 101);
  REQUIRE(b.validate());

  // Crossed
  b.apply_delta(zf::Side::Sell, 100, 1);
  REQUIRE_FALSE(b.validate());
}
