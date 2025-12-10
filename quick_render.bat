@echo off
echo ========================================
echo   Quick Render - Film Camera Dreams
echo ========================================
echo.
echo Controls:
echo   - Edit data\showcase_interactive.json
echo   - Adjust "aperture" and "focal_distance"
echo   - Run this script to re-render
echo.
echo Rendering...
cd build\Release
raytracing.exe ..\..\data\showcase_interactive.json
echo.
echo Done! Image should open automatically.
pause
