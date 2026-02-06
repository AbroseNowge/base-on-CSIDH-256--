@echo off
REM 编译交互式密钥交换演示程序

echo 正在编译交互式密钥交换演示程序...
echo.

gcc -O3 -Wall -Isrc -o interactive_key_exchange.exe ^
    interactive_key_exchange.c ^
    src/fp256.c ^
    src/edwards256.c ^
    src/edwards256_action.c ^
    src/mont_field.c ^
    src/traditional_mul.c ^
    src/rng.c ^
    -lm -lcrypt32

if %ERRORLEVEL% == 0 (
    echo.
    echo 编译成功！
    echo 运行程序: interactive_key_exchange.exe
) else (
    echo.
    echo 编译失败！
    pause
)

