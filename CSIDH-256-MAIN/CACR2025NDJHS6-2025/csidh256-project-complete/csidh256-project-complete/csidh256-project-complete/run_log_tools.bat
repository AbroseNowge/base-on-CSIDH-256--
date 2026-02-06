@echo off
chcp 65001 >nul
title CSIDH-256 日志生成和分析工具

echo.
echo ==============================================
echo CSIDH-256 日志生成和分析工具
echo ==============================================
echo.

echo 🔨 编译日志生成程序...
gcc -O3 -Isrc -o log_generator.exe log_generator.c src/mont_field.c src/optimized_montgomery_algorithm.c src/traditional_mul.c src/utils.c -lm

if %errorlevel% neq 0 (
    echo ❌ 日志生成程序编译失败！
    pause
    exit /b 1
)

echo ✅ 日志生成程序编译成功！

echo.
echo 🔨 编译日志分析程序...
gcc -O3 -o log_analyzer.exe log_analyzer.c

if %errorlevel% neq 0 (
    echo ❌ 日志分析程序编译失败！
    pause
    exit /b 1
)

echo ✅ 日志分析程序编译成功！

echo.
echo 🚀 运行日志生成程序...
echo.

log_generator.exe

if %errorlevel% neq 0 (
    echo ❌ 日志生成失败！
    pause
    exit /b 1
)

echo.
echo 📊 运行日志分析程序...
echo.

log_analyzer.exe

echo.
echo 📁 生成的日志文件:
echo - benchmark_log.txt (详细测试日志)
echo - test_results.csv (CSV格式数据)
echo - performance_data.json (JSON格式数据)
echo - performance_report.txt (性能分析报告)

echo.
echo ✅ 日志生成和分析完成！
pause


