echo.
echo === Python Orchestration Runner ===
echo.
cd python
REM Create venv if missing
if not exist ".venv\Scripts\python.exe" (
  echo [1/4] Creating venv in .venv ...
  python -m venv .venv
  if errorlevel 1 goto :fail
) else (
  echo [1/4] Venv exists: .venv
)

REM Activate venv
echo [2/4] Activating venv ...
call ".venv\Scripts\activate.bat"
if errorlevel 1 goto :fail
REM Install deps (idempotent)
echo [3/4] Installing requirements ...
python -m pip install --upgrade pip
if errorlevel 1 goto :fail
python -m pip install -r "requirements.txt"
if errorlevel 1 goto :fail
REM Run orchestration from python/ dir
echo [4/4] Running orchestration.run_bench ...
python -m orchestration.run_bench --build-dir ..\build --config ..\configs\bench.yaml --out-dir ..\out
cd ..
git add . 
git commit -m"python update"
git push