#pragma once

#include <array>
#include <cstdint>
#include <string>
#include <vector>

namespace zf {

// Fast-ish log2 histogram for latency (nanoseconds). Not perfect, but stable and simple.
class Log2Histogram {
 public:
  static constexpr std::size_t kBuckets = 64; // up to ~2^63 ns

  void add(std::uint64_t value_ns);
  void reset();

  std::uint64_t count() const { return count_; }
  double quantile(double q) const; // q in [0,1]

  std::string to_string_summary() const;

 private:
  std::array<std::uint64_t, kBuckets> buckets_{};
  std::uint64_t count_{0};
};

struct StageTimings {
  Log2Histogram ingest_ns;
  Log2Histogram normalize_ns;
  Log2Histogram book_ns;
  Log2Histogram signal_ns;
  Log2Histogram risk_ns;
  Log2Histogram send_ns;

  void reset();
};

// Utility: write histogram summary to CSV columns.
std::vector<std::string> csv_header_prefix(const std::string& prefix);
std::vector<std::string> csv_row_prefix(const std::string& prefix, const Log2Histogram& h);

} // namespace zf
