@echo off
setlocal
cd /d "%~dp0.."

set "BOARD=%~1"
set "CHIP=%~2"
set "VER=%~3"

if "%BOARD%"=="" (
    echo Usage: build-release.bat BOARD CHIP VERSION
    echo Example: build-release.bat bread-compact-wifi esp32s3 1.0.0
    echo.
    echo Common boards:
    echo   bread-compact-wifi
    echo   esp32-s3-box-3
    echo   m5stack-core-s3
    echo   xmini-c3
    pause
    exit /b 1
)
if "%CHIP%"=="" set "CHIP=esp32s3"
if "%VER%"=="" set "VER=1.0.0"

where idf.py >nul 2>nul
if errorlevel 1 (
    echo [ERROR] idf.py not found.
    echo Open "ESP-IDF PowerShell" or run export.bat first, then retry.
    pause
    exit /b 1
)

echo ==========================================
echo  Board : %BOARD%
echo  Chip  : %CHIP%
echo  Ver   : %VER%
echo ==========================================

echo [1/4] set-target %CHIP%
idf.py set-target %CHIP%
if errorlevel 1 goto fail

echo [2/4] select board %BOARD%
call idf.py -D SDKCONFIG_DEFAULTS="sdkconfig.defaults;sdkconfig.defaults.%CHIP%" reconfigure
if errorlevel 1 goto fail

echo     NOTE: make sure Board Type is '%BOARD%' in menuconfig

echo [3/4] build
idf.py build
if errorlevel 1 goto fail

echo [4/4] merge bin
idf.py merge-bin
if errorlevel 1 goto fail

if not exist "release" mkdir "release"
set "OUT=release\%BOARD%-%CHIP%-v%VER%.bin"
copy /y "build\merged-binary.bin" "%OUT%" >nul
if errorlevel 1 goto fail

echo.
echo [DONE] %OUT%
echo.
echo Next: upload this file to GitHub Releases.
pause
exit /b 0

:fail
echo.
echo [FAILED] check the error above.
pause
exit /b 1
