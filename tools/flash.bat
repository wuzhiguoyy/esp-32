@echo off
setlocal
cd /d "%~dp0"

echo ==========================================
echo   Daoxuan ESP32 Firmware Flasher
echo ==========================================
echo.

rem ---- locate esptool ----
set "ESPTOOL="
if exist "%~dp0esptool.exe" set "ESPTOOL=%~dp0esptool.exe"
if not defined ESPTOOL (
    where esptool.py >nul 2>nul
    if not errorlevel 1 set "ESPTOOL=esptool.py"
)
if not defined ESPTOOL (
    where esptool >nul 2>nul
    if not errorlevel 1 set "ESPTOOL=esptool"
)
if not defined ESPTOOL (
    python -m esptool version >nul 2>nul
    if not errorlevel 1 set "ESPTOOL=python -m esptool"
)

if not defined ESPTOOL (
    echo [ERROR] esptool not found.
    echo.
    echo Option 1: install Python 3.10 or newer, then run:
    echo     pip install esptool
    echo.
    echo Option 2: no Python needed:
    echo     download esptool.exe from
    echo     https://github.com/espressif/esptool/releases
    echo     put esptool.exe in this folder and run again.
    echo.
    pause
    exit /b 1
)

echo [OK] esptool: %ESPTOOL%

rem ---- firmware file ----
set "BIN=%~1"
if not defined BIN set "BIN=merged-binary.bin"

if not exist "%BIN%" (
    echo.
    echo [ERROR] Firmware not found: %BIN%
    echo.
    echo Put your firmware file in this folder, or drag the .bin onto this script.
    pause
    exit /b 1
)

echo [OK] firmware: %BIN%

rem ---- port ----
set "PORT=%~2"
if not defined PORT (
    echo.
    echo Available serial ports:
    %ESPTOOL% -p DUMMY read_mac 2>nul | findstr /i "COM"
    echo.
    set /p PORT=Enter port ^(example COM3^): 
)
if not defined PORT (
    echo [ERROR] No port.
    pause
    exit /b 1
)

echo.
echo ==========================================
echo   Port : %PORT%
echo   File : %BIN%
echo   Offset: 0x0
echo ==========================================
echo.
echo Prepare the board:
echo   1. Hold BOOT button
echo   2. Press RST button
echo   3. Release BOOT
echo.
pause

%ESPTOOL% --chip auto -p %PORT% -b 921600 write_flash 0x0 "%BIN%"
if errorlevel 1 (
    echo.
    echo [WARN] Fast baud failed, retry at 115200...
    %ESPTOOL% --chip auto -p %PORT% -b 115200 write_flash 0x0 "%BIN%"
)

if errorlevel 1 (
    echo.
    echo [FAILED] Flash failed.
    echo Check: USB cable (data cable), driver (CH340/CP210x), download mode.
    pause
    exit /b 1
)

echo.
echo [DONE] Flash OK. Press RST to reboot the board.
echo.
echo Next steps:
echo   1. Connect Wi-Fi (AP: Daoxuan-xxxx)
echo   2. Wait for the activation code
echo   3. Register on the website and enter the code
echo.

rem ---- open serial monitor to show activation code ----
echo [MONITOR] Opening serial monitor at 115200...
echo           Press Ctrl+C to exit.
echo.
python -c "import serial" >nul 2>nul
if errorlevel 1 (
    echo [WARN] pyserial not installed, cannot auto-open monitor.
    echo        Install with: pip install pyserial
    echo        Or open PuTTY / serial tool at 115200 on %PORT%
    pause
    exit /b 0
)

python -m serial.tools.miniterm %PORT% 115200

pause
