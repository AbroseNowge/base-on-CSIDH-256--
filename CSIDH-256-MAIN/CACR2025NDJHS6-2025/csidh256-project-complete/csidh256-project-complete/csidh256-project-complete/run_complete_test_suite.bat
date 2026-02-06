@echo off
chcp 65001 >nul
title CSIDH-256 完整测试套件

echo.
echo ==============================================
echo CSIDH-256 完整测试套件
echo ==============================================
echo.

echo 🔨 编译所有测试程序...

echo 编译测试数据生成器...
gcc -O3 -Isrc -o test_data_generator.exe test_data_generator.c src/mont_field.c src/optimized_montgomery_algorithm.c src/traditional_mul.c src/utils.c -lm

if %errorlevel% neq 0 (
    echo ❌ 测试数据生成器编译失败！
    pause
    exit /b 1
)

echo 编译密码学验证工具...
gcc -O3 -Isrc -o crypto_validator.exe crypto_validator.c src/mont_field.c src/optimized_montgomery_algorithm.c src/traditional_mul.c src/utils.c -lm

if %errorlevel% neq 0 (
    echo ❌ 密码学验证工具编译失败！
    pause
    exit /b 1
)

echo 编译日志生成器...
gcc -O3 -Isrc -o log_generator.exe log_generator.c src/mont_field.c src/optimized_montgomery_algorithm.c src/traditional_mul.c src/utils.c -lm

if %errorlevel% neq 0 (
    echo ❌ 日志生成器编译失败！
    pause
    exit /b 1
)

echo 编译日志分析器...
gcc -O3 -o log_analyzer.exe log_analyzer.c

if %errorlevel% neq 0 (
    echo ❌ 日志分析器编译失败！
    pause
    exit /b 1
)

echo ✅ 所有程序编译成功！

echo.
echo 🚀 运行完整测试套件...
echo.

echo 📊 步骤1: 生成测试数据...
test_data_generator.exe

echo.
echo 🔐 步骤2: 运行密码学验证...
crypto_validator.exe

echo.
echo 📝 步骤3: 生成性能日志...
log_generator.exe

echo.
echo 📈 步骤4: 分析测试结果...
log_analyzer.exe

echo.
echo 📁 生成的测试文件:
echo - test_datasets.txt (测试数据集)
echo - comprehensive_test_log.txt (完整测试日志)
echo - crypto_validation_log.txt (密码学验证日志)
echo - performance_benchmark_log.txt (性能基准日志)
echo - benchmark_log.txt (详细测试日志)
echo - test_results.csv (CSV格式数据)
echo - performance_data.json (JSON格式数据)
echo - performance_report.txt (性能分析报告)
echo - CRYPTO_TEST_VECTORS.md (密码学测试向量)

echo.
echo ✅ 完整测试套件运行完成！
echo.
echo 📋 测试总结:
echo - 测试数据生成: 完成
echo - 密码学验证: 完成
echo - 性能测试: 完成
echo - 日志分析: 完成
echo.
pause


