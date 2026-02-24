#pragma once

#include <cstdint>

namespace zf {

std::uint64_t now_ns();

struct Timer {
  std::uint64_t start_ns{0};
  void reset();
  std::uint64_t elapsed_ns() const;
};

} // namespace zf
