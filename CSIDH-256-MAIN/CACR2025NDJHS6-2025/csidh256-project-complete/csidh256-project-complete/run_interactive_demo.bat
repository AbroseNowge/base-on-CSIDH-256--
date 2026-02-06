@echo off
chcp 65001 >nul
title CSIDH-256 交互式演示程序

echo.
echo ==============================================
echo CSIDH-256 交互式演示程序
echo ==============================================
echo.

echo 🔨 编译交互式演示程序...
gcc -O3 -Wall -Wno-unused-const-variable -march=native -mtune=native -Isrc -o interactive_demo_program.exe interactive_demo_program.c src/mont_field.c src/optimized_montgomery_algorithm.c src/traditional_mul.c src/utils.c -lm

if %errorlevel% neq 0 (
    echo ❌ 编译失败！
    pause
    exit /b 1
)

echo ✅ 编译成功！
echo.
echo 🚀 启动交互式演示程序...
echo.

interactive_demo_program.exe

echo.
echo 👋 演示程序已结束
pause



