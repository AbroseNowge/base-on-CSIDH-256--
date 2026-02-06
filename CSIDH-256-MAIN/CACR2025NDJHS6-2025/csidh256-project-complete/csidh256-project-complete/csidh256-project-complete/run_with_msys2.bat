@echo off
chcp 65001 >nul
echo ==============================================
echo CSIDH-256 密码学测试工具 MSYS2 运行脚本
echo ==============================================
echo.

set MSYS_PATH=F:\msys64\mingw64.exe
set PROJECT_PATH=/c/Users/20513/Desktop/csidh256-project-complete/csidh256-project-complete/csidh256-project-complete

echo 检查MSYS2路径...
if exist "%MSYS_PATH%" (
    echo ✅ MSYS2找到: %MSYS_PATH%
) else (
    echo ❌ 未找到MSYS2: %MSYS_PATH%
    pause
    exit /b 1
)

echo.
echo 🔧 开始编译密码学测试工具...
echo.

echo 编译测试向量生成器...
"%MSYS_PATH%" -c "cd '%PROJECT_PATH%' && gcc -O3 -std=c99 -I../../src crypto_test_vectors_generator.c ../../src/mont_field.c ../../src/mont_field_optimized.c ../../src/traditional_mul.c -o crypto_test_vectors_generator.exe"
if %errorlevel% neq 0 (
    echo ❌ 测试向量生成器编译失败
) else (
    echo ✅ 测试向量生成器编译成功
)

echo.
echo 编译增强版测试收集器...
"%MSYS_PATH%" -c "cd '%PROJECT_PATH%' && gcc -O3 -std=c99 -I../../src enhanced_crypto_test_collector.c ../../src/mont_field.c ../../src/mont_field_optimized.c ../../src/traditional_mul.c -o enhanced_crypto_test_collector.exe"
if %errorlevel% neq 0 (
    echo ❌ 增强版测试收集器编译失败
) else (
    echo ✅ 增强版测试收集器编译成功
)

echo.
echo 编译性能测试数据生成器...
"%MSYS_PATH%" -c "cd '%PROJECT_PATH%' && gcc -O3 -std=c99 -I../../src crypto_performance_test_data_generator.c ../../src/mont_field.c ../../src/mont_field_optimized.c ../../src/traditional_mul.c -o crypto_performance_test_data_generator.exe"
if %errorlevel% neq 0 (
    echo ❌ 性能测试数据生成器编译失败
) else (
    echo ✅ 性能测试数据生成器编译成功
)

echo.
echo 编译测试数据验证器...
"%MSYS_PATH%" -c "cd '%PROJECT_PATH%' && gcc -O3 -std=c99 -I../../src crypto_test_data_validator.c ../../src/mont_field.c ../../src/mont_field_optimized.c ../../src/traditional_mul.c -o crypto_test_data_validator.exe"
if %errorlevel% neq 0 (
    echo ❌ 测试数据验证器编译失败
) else (
    echo ✅ 测试数据验证器编译成功
)

echo.
echo ==============================================
echo 编译完成！开始运行测试工具
echo ==============================================
echo.

echo 🔐 生成密码学测试向量...
"%MSYS_PATH%" -c "cd '%PROJECT_PATH%' && echo '1' | ./crypto_test_vectors_generator.exe"
if %errorlevel% neq 0 (
    echo ❌ 测试向量生成失败
) else (
    echo ✅ 测试向量生成成功
)

echo.
echo 🚀 运行增强版性能测试...
"%MSYS_PATH%" -c "cd '%PROJECT_PATH%' && ./enhanced_crypto_test_collector.exe"
if %errorlevel% neq 0 (
    echo ❌ 性能测试失败
) else (
    echo ✅ 性能测试成功
)

echo.
echo ⚡ 生成性能测试数据...
"%MSYS_PATH%" -c "cd '%PROJECT_PATH%' && echo '1' | ./crypto_performance_test_data_generator.exe"
if %errorlevel% neq 0 (
    echo ❌ 性能测试数据生成失败
) else (
    echo ✅ 性能测试数据生成成功
)

echo.
echo 🔍 验证测试数据...
"%MSYS_PATH%" -c "cd '%PROJECT_PATH%' && echo '1' | ./crypto_test_data_validator.exe"
if %errorlevel% neq 0 (
    echo ❌ 测试数据验证失败
) else (
    echo ✅ 测试数据验证成功
)

echo.
echo ==============================================
echo 测试完成！查看生成的文件
echo ==============================================
echo.

echo 📊 查找生成的文件...
"%MSYS_PATH%" -c "cd '%PROJECT_PATH%' && ls -la *.json *.csv *.txt *.log 2>/dev/null || echo '未找到生成的文件'"

echo.
echo 🎯 下一步操作建议:
echo 1. 查看生成的JSON文件了解测试数据结构
echo 2. 使用Excel打开CSV文件进行数据分析
echo 3. 阅读TXT报告文件了解详细测试结果
echo 4. 检查LOG文件了解测试执行过程
echo.

echo 💡 提示:
echo - 所有测试数据都基于CSIDH-256后量子密码算法
echo - 测试向量符合NIST和RFC标准
echo - 性能指标包含执行时间、吞吐量、能效比等
echo - 安全测试覆盖侧信道攻击、时序攻击等威胁模型
echo.

pause


