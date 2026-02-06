@echo off
chcp 65001 >nul
echo ==============================================
echo CSIDH-256 详细性能分析工具
echo ==============================================
echo.

set MSYS_PATH=F:\msys64\mingw64.exe
set PROJECT_PATH=/c/Users/20513/Desktop/csidh256-project-complete/csidh256-project-complete/csidh256-project-complete

echo 🔧 编译详细性能分析工具...
echo.
"%MSYS_PATH%" -c "cd '%PROJECT_PATH%' && gcc -O3 -std=c99 -o detailed_performance_analysis.exe detailed_performance_analysis.c"
if %errorlevel% neq 0 (
    echo ❌ 编译失败
    pause
    exit /b 1
) else (
    echo ✅ 编译成功
)

echo.
echo 🚀 运行详细性能分析...
echo.
"%MSYS_PATH%" -c "cd '%PROJECT_PATH%' && ./detailed_performance_analysis.exe"

echo.
echo ==============================================
echo 详细性能分析完成
echo ==============================================
echo.

pause

