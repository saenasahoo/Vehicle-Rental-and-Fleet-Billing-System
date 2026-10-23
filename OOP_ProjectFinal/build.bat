@echo off
REM Builds the project on Windows (needs g++ / MinGW on the PATH).
g++ main.cpp -o VehicleRental.exe
if %errorlevel% neq 0 (
    echo Build FAILED.
) else (
    echo Build OK - now run VehicleRental.exe
)
pause
