@echo off
chcp 65001 >nul
title CSIDH-256 测试数据收集

echo.
echo ==============================================
echo CSIDH-256 测试数据收集程序
echo ==============================================
echo.

echo 🔨 编译测试数据收集程序...
gcc -O3 -Wall -Wno-unused-const-variable -march=native -mtune=native -Isrc -o test_data_collector.exe test_data_collector.c src/mont_field.c src/mont_field_simple_optimized.c src/traditional_mul.c src/utils.c -lm

if %errorlevel% neq 0 (
    echo ❌ 编译失败！
    pause
    exit /b 1
)

echo ✅ 编译成功！
echo.
echo 🚀 运行测试数据收集...
echo.

test_data_collector.exe

echo.
echo 📊 测试数据收集完成！
echo.
echo 生成的文件:
echo - test_results.csv (CSV格式测试数据)
echo - performance_data.json (JSON格式性能数据)
echo - benchmark_log.txt (详细测试日志)
echo.
pause



