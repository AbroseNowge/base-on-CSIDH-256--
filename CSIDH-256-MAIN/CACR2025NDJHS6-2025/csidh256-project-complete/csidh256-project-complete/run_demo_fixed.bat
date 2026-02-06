@echo off
chcp 65001 >nul
title CSIDH-256 后量子密码算法优化项目

echo.
echo ==============================================
echo 🎯 CSIDH-256 后量子密码算法优化项目
echo ==============================================
echo.

echo 🔨 正在编译项目...
echo 检查编译环境...

where gcc >nul 2>&1
if %errorlevel% neq 0 (
    echo ❌ 未找到GCC编译器！
    echo 请安装MinGW-w64或MSYS2
    pause
    exit /b 1
)

echo ✅ 找到GCC编译器
echo 开始编译...

gcc -O3 -Wall -Wno-unused-const-variable -march=native -mtune=native -o demo_windows.exe demo_windows.c src/mont_field_simple_optimized.c src/mont_field.c src/traditional_mul.c src/utils.c

if %errorlevel% neq 0 (
    echo ❌ 编译失败！
    echo 请检查源文件是否存在
    pause
    exit /b 1
)

echo ✅ 编译成功！
echo.
echo 🚀 启动演示程序...
echo.

demo_windows.exe

echo.
echo 👋 演示程序已结束
pause



