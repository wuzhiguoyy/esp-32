@echo off
REM Flash merged firmware (offset 0x0) to an ESP32-S3 board.
setlocal enabledelayedexpansion

set PORT=%~1
if "%PORT%"=="" set PORT=COM3

if not exist "%~dp0firmware.bin" (
    echo [ERROR] firmware.bin not found next to this script.
    pause
    exit /b 1
)

where python >nul 2>nul
if errorlevel 1 (
    echo [ERROR] Python not found. Install Python 3.10+ first.
    pause
    exit /b 1
)

echo Flashing %PORT% ...
python -m esptool --chip esp32s3 -p %PORT% -b 921600 --before default-reset --after hard-reset write-flash 0x0 "%~dp0firmware.bin"
if errorlevel 1 (
    echo.
    echo Retry at 115200 ...
    python -m esptool --chip esp32s3 -p %PORT% -b 115200 --before default-reset --after hard-reset write-flash 0x0 "%~dp0firmware.bin"
)

if errorlevel 1 (
    echo.
    echo [FAILED] Check USB cable / driver / COM port.
    pause
    exit /b 1
)

echo.
echo [DONE] Now open a serial monitor at 115200 to read the activation code.
pause
