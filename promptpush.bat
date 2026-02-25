set "MSG=%*"
if "%MSG%"=="" set "MSG=reg push"
git add .
git commit -m "%MSG%"
git push