#pragma once

#include <memory>
#include <string_view>

#include <spdlog/logger.h>

namespace zf {

// Logging is intentionally kept off the engine hot-path. Prefer counters/metrics there.
// Enable at build time with -DZF_ENABLE_LOGGING=1.

std::shared_ptr<spdlog::logger> logger();
void init_logging(std::string_view level);

} // namespace zf

#if defined(ZF_ENABLE_LOGGING)
  #define ZF_LOG_TRACE(...) ::zf::logger()->trace(__VA_ARGS__)
  #define ZF_LOG_DEBUG(...) ::zf::logger()->debug(__VA_ARGS__)
  #define ZF_LOG_INFO(...)  ::zf::logger()->info(__VA_ARGS__)
  #define ZF_LOG_WARN(...)  ::zf::logger()->warn(__VA_ARGS__)
  #define ZF_LOG_ERROR(...) ::zf::logger()->error(__VA_ARGS__)
#else
  #define ZF_LOG_TRACE(...) (void)0
  #define ZF_LOG_DEBUG(...) (void)0
  #define ZF_LOG_INFO(...)  (void)0
  #define ZF_LOG_WARN(...)  (void)0
  #define ZF_LOG_ERROR(...) (void)0
#endif
