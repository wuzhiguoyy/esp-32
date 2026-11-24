@echo off
setlocal enabledelayedexpansion
cd /d "%~dp0"

set "PROXY=http://127.0.0.1:7890"
set "GH=https://github.com/wuzhiguoyy/esp-32.git"
set "GITEE=https://gitee.com/wuzhiguo/esp-32.git"

set "MSG=%~1"
if "%MSG%"=="" set "MSG=chore: update"

echo ============================================
echo  Daoxuan ESP32 - commit and push
echo ============================================
echo.

echo [1/4] git add -A
git add -A

git diff --cached --quiet
if errorlevel 1 goto do_commit
echo [2/4] nothing to commit
goto push_github

:do_commit
echo [2/4] commit: %MSG%
git commit -m "%MSG%"

:push_github
echo.
echo [3/4] push GitHub (proxy %PROXY%)
set "TRY=0"
:retry_gh
set /a TRY+=1
git -c http.proxy=%PROXY% -c https.proxy=%PROXY% -c http.version=HTTP/1.1 push %GH% main
if not errorlevel 1 goto gh_ok
if %TRY% GEQ 3 goto gh_fail
echo     retry %TRY% failed, waiting...
ping -n 3 127.0.0.1 >nul
goto retry_gh

:gh_ok
echo GitHub push OK
goto push_gitee

:gh_fail
echo GitHub push FAILED, check proxy

:push_gitee
echo.
echo [4/4] push Gitee (direct)
git -c http.proxy= -c https.proxy= push %GITEE% main
if errorlevel 1 echo Gitee push FAILED
if not errorlevel 1 echo Gitee push OK

echo.
echo ============================================
echo  Recent commits
git log --oneline -3
echo ============================================
pause
