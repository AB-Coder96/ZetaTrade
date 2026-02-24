#include <zetaforge/util/metrics.hpp>

#include <cmath>
#include <sstream>

namespace zf {

namespace {
inline std::size_t bucket_index(std::uint64_t v) {
  if (v == 0) return 0;
#if defined(__GNUG__)
  return static_cast<std::size_t>(63 - __builtin_clzll(v));
#else
  // portable fallback
  std::size_t idx = 0;
  while (v >>= 1) ++idx;
  return idx;
#endif
}
}

void Log2Histogram::add(std::uint64_t value_ns) {
  const auto idx = bucket_index(value_ns);
  buckets_[idx] += 1;
  count_ += 1;
}

void Log2Histogram::reset() {
  buckets_.fill(0);
  count_ = 0;
}

double Log2Histogram::quantile(double q) const {
  if (count_ == 0) return 0.0;
  if (q <= 0.0) q = 0.0;
  if (q >= 1.0) q = 1.0;

  const auto target = static_cast<std::uint64_t>(std::ceil(q * static_cast<double>(count_)));
  std::uint64_t cum = 0;
  for (std::size_t i = 0; i < kBuckets; ++i) {
    cum += buckets_[i];
    if (cum >= target) {
      // Return bucket midpoint in ns: [2^i, 2^(i+1))
      const double lo = std::ldexp(1.0, static_cast<int>(i));
      const double hi = std::ldexp(1.0, static_cast<int>(i + 1));
      return 0.5 * (lo + hi);
    }
  }
  return std::ldexp(1.0, 63);
}

std::string Log2Histogram::to_string_summary() const {
  std::ostringstream oss;
  oss << "n=" << count_;
  oss << " p50~" << quantile(0.50) << "ns";
  oss << " p99~" << quantile(0.99) << "ns";
  oss << " p999~" << quantile(0.999) << "ns";
  return oss.str();
}

void StageTimings::reset() {
  ingest_ns.reset();
  normalize_ns.reset();
  book_ns.reset();
  signal_ns.reset();
  risk_ns.reset();
  send_ns.reset();
}

std::vector<std::string> csv_header_prefix(const std::string& prefix) {
  return {
      prefix + "_n",
      prefix + "_p50_ns",
      prefix + "_p99_ns",
      prefix + "_p999_ns",
  };
}

std::vector<std::string> csv_row_prefix(const std::string& prefix, const Log2Histogram& h) {
  (void)prefix;
  return {
      std::to_string(h.count()),
      std::to_string(static_cast<std::uint64_t>(h.quantile(0.50))),
      std::to_string(static_cast<std::uint64_t>(h.quantile(0.99))),
      std::to_string(static_cast<std::uint64_t>(h.quantile(0.999))),
  };
}

} // namespace zf
