set "MSG=%*"
if "%MSG%"=="" set "MSG=reg push"
set /p MSG=Enter commit message (leave blank for "reg push"): 
git add .
git commit -m "%MSG%"
git push