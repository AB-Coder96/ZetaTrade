cmake_minimum_required(VERSION 3.20)
project(Zetaforge VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(ZF_ENABLE_TESTS "Build unit tests" ON)
option(ZF_ENABLE_WARNINGS "Enable strict warnings" ON)
option(ZF_ENABLE_LTO "Enable link-time optimization (Release)" OFF)

if (ZF_ENABLE_WARNINGS)
  if (MSVC)
    add_compile_options(/W4 /permissive-)
  else()
    add_compile_options(-Wall -Wextra -Wpedantic -Wconversion -Wshadow)
  endif()
endif()

if (ZF_ENABLE_LTO)
  include(CheckIPOSupported)
  check_ipo_supported(RESULT ipo_supported OUTPUT ipo_error)
  if (ipo_supported)
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE)
  else()
    message(WARNING "IPO/LTO not supported: ${ipo_error}")
  endif()
endif()

include(FetchContent)
set(FETCHCONTENT_QUIET OFF)

# Dependencies
FetchContent_Declare(
  fmt
  GIT_REPOSITORY https://github.com/fmtlib/fmt.git
  GIT_TAG 10.2.1
)
FetchContent_MakeAvailable(fmt)

FetchContent_Declare(
  spdlog
  GIT_REPOSITORY https://github.com/gabime/spdlog.git
  GIT_TAG v1.13.0
)
FetchContent_MakeAvailable(spdlog)

FetchContent_Declare(
  CLI11
  GIT_REPOSITORY https://github.com/CLIUtils/CLI11.git
  GIT_TAG v2.4.2
)
FetchContent_MakeAvailable(CLI11)

FetchContent_Declare(
  nlohmann_json
  GIT_REPOSITORY https://github.com/nlohmann/json.git
  GIT_TAG v3.11.3
)
FetchContent_MakeAvailable(nlohmann_json)

FetchContent_Declare(
  yaml_cpp
  GIT_REPOSITORY https://github.com/jbeder/yaml-cpp.git
  GIT_TAG 0.8.0
)
FetchContent_MakeAvailable(yaml_cpp)

# Core library
add_library(zetaforge
  cpp/src/util/logging.cpp
  cpp/src/util/metrics.cpp
  cpp/src/util/time.cpp
  cpp/src/util/config.cpp
  cpp/src/ingest/jsonl_reader.cpp
  cpp/src/book/order_book.cpp
  cpp/src/sim/exchange_sim.cpp
  cpp/src/oms/order_gateway.cpp
  cpp/src/risk/risk_gate.cpp
  cpp/src/bench/bench_runner.cpp
)

target_include_directories(zetaforge
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/cpp/include>
)

target_link_libraries(zetaforge
  PUBLIC
    fmt::fmt
    spdlog::spdlog
    CLI11::CLI11
    nlohmann_json::nlohmann_json
    yaml-cpp
)

target_compile_definitions(zetaforge
  PUBLIC
    $<$<CONFIG:Release>:ZF_RELEASE_BUILD>
)

# Apps
add_executable(Zetaforge_replay cpp/apps/replay_main.cpp)
add_executable(Zetaforge_trade  cpp/apps/trade_main.cpp)
add_executable(Zetaforge_bench  cpp/apps/bench_main.cpp)

foreach(app Zetaforge_replay Zetaforge_trade Zetaforge_bench)
  target_link_libraries(${app} PRIVATE zetaforge)
endforeach()

# Tests
if (ZF_ENABLE_TESTS)
  enable_testing()

  FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.5.4
  )
  FetchContent_MakeAvailable(Catch2)

  add_executable(zetaforge_tests
    cpp/tests/test_order_book.cpp
    cpp/tests/test_oms_state_machine.cpp
    cpp/tests/test_risk_gate.cpp
  )
  target_link_libraries(zetaforge_tests PRIVATE zetaforge Catch2::Catch2WithMain)

  include(CTest)
  include(Catch)
  catch_discover_tests(zetaforge_tests)
endif()
