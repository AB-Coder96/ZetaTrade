#include <iostream>
#include <CLI/CLI.hpp>
#include <zetaforge/bench/bench_runner.hpp>
#include <zetaforge/config.hpp>
#include <zetaforge/util/logging.hpp>

int main(int argc, char** argv) {
  CLI::App app{"Zetaforge_bench — microbenchmarks"};

  std::string config_path;
  std::string out_path;
  std::string log_level = "info";

  app.add_option("--config", config_path, "YAML config")->required();
  app.add_option("--out", out_path, "Output CSV path")->required();
  app.add_option("--log-level", log_level, "Log level (trace/debug/info/warn/error)");

  CLI11_PARSE(app, argc, argv);

  zf::init_logging(log_level);

  auto cfg = zf::load_bench_config_yaml(config_path);
  auto r = zf::run_replay_book_bench(cfg);
  zf::write_bench_csv(out_path, r);

  std::cout << "scenario=replay_to_book\n";
  std::cout << "events=" << r.events << "\n";
  std::cout << "seconds=" << r.seconds << "\n";
  std::cout << "throughput_eps=" << r.throughput_eps << "\n";
  std::cout << "book_latency=" << r.book_update_ns.to_string_summary() << "\n";

  return 0;
}
