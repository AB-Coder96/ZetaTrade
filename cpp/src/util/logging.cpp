#include <zetaforge/util/logging.hpp>

#include <mutex>

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

namespace zf {

namespace {
std::once_flag g_once;
std::shared_ptr<spdlog::logger> g_logger;
}

std::shared_ptr<spdlog::logger> logger() {
  std::call_once(g_once, [] {
    auto sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    sink->set_pattern("%Y-%m-%d %H:%M:%S.%e [%^%l%$] %v");
    g_logger = std::make_shared<spdlog::logger>("zetaforge", sink);
    g_logger->set_level(spdlog::level::info);
    g_logger->flush_on(spdlog::level::warn);
    spdlog::register_logger(g_logger);
  });
  return g_logger;
}

void init_logging(std::string_view level) {
  auto lg = logger();
  auto lvl = spdlog::level::from_str(std::string(level));
  lg->set_level(lvl);
}

} // namespace zf
