@echo off
chcp 65001 >nul
title CSIDH-256 测试数据生成器

echo.
echo ==============================================
echo CSIDH-256 测试数据生成器
echo ==============================================
echo.

echo 🔨 编译测试数据生成程序...
gcc -O3 -Isrc -o test_data_generator.exe test_data_generator.c src/mont_field.c src/optimized_montgomery_algorithm.c src/traditional_mul.c src/utils.c -lm

if %errorlevel% neq 0 (
    echo ❌ 编译失败！
    pause
    exit /b 1
)

echo ✅ 编译成功！

echo.
echo 🚀 运行测试数据生成程序...
echo.

test_data_generator.exe

echo.
echo 📁 生成的文件:
echo - test_datasets.txt (测试数据集)
echo - comprehensive_test_log.txt (完整测试日志)
echo - CRYPTO_TEST_VECTORS.md (密码学测试向量)

echo.
echo ✅ 测试数据生成完成！
pause


