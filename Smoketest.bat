build\Zetaforge_replay.exe --input data\sample_day.jsonl --seed 42 --speed 10
build\Zetaforge_trade.exe  --config configs\demo_strategy.yaml --seed 42
build\Zetaforge_bench.exe  --config configs\bench.yaml --out out\bench.csv
git add .
git commit -m "Smoke test"
git push