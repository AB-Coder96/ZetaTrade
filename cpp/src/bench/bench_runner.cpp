#include <zetaforge/bench/bench_runner.hpp>

#include <fstream>

#include <zetaforge/book/l2_book.hpp>
#include <zetaforge/ingest/jsonl_reader.hpp>
#include <zetaforge/util/time.hpp>

namespace zf {

BenchResult run_replay_book_bench(const BenchConfig& cfg) {
  auto events = read_jsonl_events(cfg.input_jsonl);

  L2Book book;
  BenchResult r;

  // Warmup
  std::uint64_t warm = 0;
  for (const auto& e : events) {
    if (e.type == EventType::BookDelta) {
      book.apply_delta(e.side, e.price, e.qty);
      (void)book.validate();
      if (++warm >= cfg.warmup_events) break;
    }
  }

  // Measure
  const auto start = now_ns();
  std::uint64_t measured = 0;
  for (const auto& e : events) {
    if (e.type != EventType::BookDelta) continue;
    const auto t0 = now_ns();
    book.apply_delta(e.side, e.price, e.qty);
    const auto t1 = now_ns();
    r.book_update_ns.add(t1 - t0);
    measured++;
    if (measured >= cfg.measure_events) break;
  }
  const auto end = now_ns();

  r.events = measured;
  r.seconds = static_cast<double>(end - start) / 1e9;
  r.throughput_eps = (r.seconds > 0.0) ? static_cast<double>(measured) / r.seconds : 0.0;
  return r;
}

void write_bench_csv(const std::string& out_path, const BenchResult& r) {
  std::ofstream out(out_path);
  if (!out) throw std::runtime_error("Failed to open output: " + out_path);

  out << "scenario,events,seconds,throughput_eps,book_p50_ns,book_p99_ns,book_p999_ns\n";
  out << "replay_to_book," << r.events << "," << r.seconds << "," << r.throughput_eps << ","
      << static_cast<std::uint64_t>(r.book_update_ns.quantile(0.50)) << ","
      << static_cast<std::uint64_t>(r.book_update_ns.quantile(0.99)) << ","
      << static_cast<std::uint64_t>(r.book_update_ns.quantile(0.999)) << "\n";
}

} // namespace zf
