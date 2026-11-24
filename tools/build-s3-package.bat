@echo off
REM Build ESP32-S3 universal firmware package.
REM Requires ESP-IDF v6.1 environment (run export.bat first).

setlocal
set ROOT=%~dp0..
set OUT=%ROOT%\dist\esp32s3-universal

echo [1/4] set-target esp32s3
call idf.py set-target esp32s3
if errorlevel 1 goto :fail

echo [2/4] build
call idf.py build
if errorlevel 1 goto :fail

echo [3/4] merge-bin
call idf.py merge-bin
if errorlevel 1 goto :fail

echo [4/4] package
if not exist "%OUT%" mkdir "%OUT%"
copy /y "%ROOT%\build\merged-binary.bin" "%OUT%\firmware.bin" >nul
copy /y "%~dp0flash-universal.bat" "%OUT%\flash.bat" >nul
copy /y "%~dp0..\docs\USAGE_zh.txt" "%OUT%\使用说明.txt" >nul
if exist "%ROOT%\tools\esptool.exe" copy /y "%ROOT%\tools\esptool.exe" "%OUT%\esptool.exe" >nul

echo.
echo DONE: %OUT%
echo Upload all files in that folder to GitHub Releases.
goto :eof

:fail
echo.
echo FAILED - see errors above
exit /b 1
