#include <catch2/catch_test_macros.hpp>
#include <zetaforge/book/l2_book.hpp>

TEST_CASE("L2Book best bid/ask, levels, and validation") {
  zf::L2Book b;

  SECTION("Empty book") {
    REQUIRE_FALSE(b.best_bid().has_value());
    REQUIRE_FALSE(b.best_ask().has_value());
    REQUIRE(b.validate());
    REQUIRE(b.level_qty(zf::Side::Buy, 100) == 0);
    REQUIRE(b.level_qty(zf::Side::Sell, 100) == 0);
  }

  SECTION("apply_delta sets and removes levels") {
    b.apply_delta(zf::Side::Buy, 99, 10);
    b.apply_delta(zf::Side::Sell, 101, 12);

    REQUIRE(b.level_qty(zf::Side::Buy, 99) == 10);
    REQUIRE(b.level_qty(zf::Side::Sell, 101) == 12);

    // qty <= 0 removes the level
    b.apply_delta(zf::Side::Buy, 99, 0);
    b.apply_delta(zf::Side::Sell, 101, -5);

    REQUIRE(b.level_qty(zf::Side::Buy, 99) == 0);
    REQUIRE(b.level_qty(zf::Side::Sell, 101) == 0);

    REQUIRE(b.validate());
  }

  SECTION("Best bid/ask reflect top-of-book") {
    // Multiple bid levels: best bid should be the highest price.
    b.apply_delta(zf::Side::Buy, 99, 10);
    b.apply_delta(zf::Side::Buy, 101, 5);
    REQUIRE(b.best_bid().has_value());
    REQUIRE(b.best_bid().value() == 101);

    // Multiple ask levels: best ask should be the lowest price.
    b.apply_delta(zf::Side::Sell, 105, 7);
    b.apply_delta(zf::Side::Sell, 103, 4);
    REQUIRE(b.best_ask().has_value());
    REQUIRE(b.best_ask().value() == 103);

    REQUIRE(b.validate());
  }

  SECTION("Locked market (bb == ba) is allowed") {
    b.apply_delta(zf::Side::Buy, 100, 1);
    b.apply_delta(zf::Side::Sell, 100, 1);

    REQUIRE(b.best_bid().has_value());
    REQUIRE(b.best_ask().has_value());
    REQUIRE(b.best_bid().value() == 100);
    REQUIRE(b.best_ask().value() == 100);

    // With the updated rule: locked is valid, only crossed is invalid.
    REQUIRE(b.validate());
  }

  SECTION("Crossed market (bb > ba) is invalid") {
    zf::L2Book x;
    x.apply_delta(zf::Side::Buy, 101, 1);
    x.apply_delta(zf::Side::Sell, 100, 1);

    REQUIRE(x.best_bid().has_value());
    REQUIRE(x.best_ask().has_value());
    REQUIRE(x.best_bid().value() == 101);
    REQUIRE(x.best_ask().value() == 100);

    REQUIRE_FALSE(x.validate());
  }

  SECTION("Removing top levels updates best bid/ask") {
    // bids: 101 is best, then remove it -> best becomes 99
    b.apply_delta(zf::Side::Buy, 99, 10);
    b.apply_delta(zf::Side::Buy, 101, 5);
    REQUIRE(b.best_bid().value() == 101);
    b.apply_delta(zf::Side::Buy, 101, 0);
    REQUIRE(b.best_bid().value() == 99);

    // asks: 103 is best, then remove it -> best becomes 105
    b.apply_delta(zf::Side::Sell, 105, 7);
    b.apply_delta(zf::Side::Sell, 103, 4);
    REQUIRE(b.best_ask().value() == 103);
    b.apply_delta(zf::Side::Sell, 103, 0);
    REQUIRE(b.best_ask().value() == 105);

    REQUIRE(b.validate());
  }
}