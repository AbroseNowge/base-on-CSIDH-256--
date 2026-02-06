@echo off
chcp 65001 >nul
title CSIDH-256 性能对比测试

echo.
echo ==============================================
echo CSIDH-256 性能对比测试
echo ==============================================
echo.

echo 🔨 编译性能对比测试程序...
gcc -O3 -Wall -Wno-unused-const-variable -march=native -mtune=native -Isrc -o performance_comparison_test.exe performance_comparison_test.c src/mont_field.c src/optimized_montgomery_algorithm.c src/traditional_mul.c src/utils.c -lm

if %errorlevel% neq 0 (
    echo ❌ 编译失败！
    pause
    exit /b 1
)

echo ✅ 编译成功！
echo.
echo 🚀 运行性能对比测试...
echo.

performance_comparison_test.exe

echo.
echo 👋 测试完成！
pause



