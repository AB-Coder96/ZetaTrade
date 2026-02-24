#include <zetaforge/util/time.hpp>

#include <chrono>

namespace zf {

std::uint64_t now_ns() {
  using clock = std::chrono::steady_clock;
  return static_cast<std::uint64_t>(
      std::chrono::duration_cast<std::chrono::nanoseconds>(clock::now().time_since_epoch()).count());
}

void Timer::reset() { start_ns = now_ns(); }

std::uint64_t Timer::elapsed_ns() const {
  auto n = now_ns();
  return n >= start_ns ? (n - start_ns) : 0ULL;
}

} // namespace zf
