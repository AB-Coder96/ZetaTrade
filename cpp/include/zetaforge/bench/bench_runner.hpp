#pragma once

#include <cstdint>
#include <string>

#include <zetaforge/config.hpp>
#include <zetaforge/util/metrics.hpp>

namespace zf {

struct BenchResult {
  std::uint64_t events{0};
  double seconds{0.0};
  double throughput_eps{0.0};
  Log2Histogram book_update_ns;
};

BenchResult run_replay_book_bench(const BenchConfig& cfg);

// Write a one-row CSV with standard columns.
void write_bench_csv(const std::string& out_path, const BenchResult& r);

} // namespace zf
