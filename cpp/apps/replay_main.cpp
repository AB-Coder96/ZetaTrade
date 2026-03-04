#include <cstdint>
#include <iostream>
#include <CLI/CLI.hpp>
#include <zetaforge/book/l2_book.hpp>
#include <zetaforge/ingest/jsonl_reader.hpp>
#include <zetaforge/util/logging.hpp>

namespace {
std::uint64_t checksum_book(const zf::L2Book& /*book*/) {
  // Placeholder: if you extend L2Book to expose iterators, compute a deterministic hash here.
  return 0ULL;
}
}

int main(int argc, char** argv) {
  CLI::App app{"Zetaforge_replay — deterministic replay to L2 book"};

  std::string input;
  std::uint64_t seed = 42;
  double speed = 1.0;
  std::string log_level = "info";

  app.add_option("--input", input, "Input JSONL (normalized events)")->required();
  app.add_option("--seed", seed, "Seed (reserved for future stochastic models)");
  app.add_option("--speed", speed, "Replay speed multiplier (reserved for wall-clock pacing)");
  app.add_option("--log-level", log_level, "Log level (trace/debug/info/warn/error)");

  CLI11_PARSE(app, argc, argv);

  (void)seed;
  (void)speed;

  zf::init_logging(log_level);

  auto events = zf::read_jsonl_events(input);
  zf::L2Book book;

  std::uint64_t n = 0;
  for (const auto& e : events) {
    if (e.type == zf::EventType::BookDelta) {
      book.apply_delta(e.side, e.price, e.qty);
      if (!book.validate()) {
        std::cerr << "Book validation failed at event=" << n << "\n";
        return 2;
      }
    }
    ++n;
  }

  auto bb = book.best_bid();
  auto ba = book.best_ask();

  std::cout << "events=" << n << "\n";
  std::cout << "best_bid=" << (bb ? std::to_string(*bb) : "NA") << "\n";
  std::cout << "best_ask=" << (ba ? std::to_string(*ba) : "NA") << "\n";
  std::cout << "checksum=" << checksum_book(book) << "\n";

  return 0;
}
