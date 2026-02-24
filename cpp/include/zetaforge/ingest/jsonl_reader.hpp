#pragma once

#include <string>
#include <vector>

#include <zetaforge/types.hpp>

namespace zf {

// Reads a small JSONL file into memory. For large datasets, prefer streaming.
std::vector<MarketEvent> read_jsonl_events(const std::string& path);

} // namespace zf
