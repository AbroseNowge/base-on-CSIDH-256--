@echo off
chcp 65001 >nul
echo ==============================================
echo CSIDH-256 内置测试数据运行脚本
echo ==============================================
echo.

set MSYS_PATH=F:\msys64\mingw64.exe
set PROJECT_PATH=/c/Users/20513/Desktop/csidh256-project-complete/csidh256-project-complete/csidh256-project-complete

echo 🔍 检查MSYS2环境...
if exist "%MSYS_PATH%" (
    echo ✅ MSYS2找到: %MSYS_PATH%
) else (
    echo ❌ 未找到MSYS2: %MSYS_PATH%
    pause
    exit /b 1
)

echo.
echo 🔧 编译内置测试工具...
echo.

echo 编译测试数据收集器...
"%MSYS_PATH%" -c "cd '%PROJECT_PATH%' && gcc -O3 -std=c99 -I../../src test_data_collector.c ../../src/mont_field.c ../../src/mont_field_optimized.c ../../src/traditional_mul.c -o test_data_collector.exe"
if %errorlevel% neq 0 (
    echo ❌ 测试数据收集器编译失败
) else (
    echo ✅ 测试数据收集器编译成功
)

echo.
echo 编译测试数据生成器...
"%MSYS_PATH%" -c "cd '%PROJECT_PATH%' && gcc -O3 -std=c99 -I../../src test_data_generator.c ../../src/mont_field.c ../../src/mont_field_optimized.c ../../src/traditional_mul.c -o test_data_generator.exe"
if %errorlevel% neq 0 (
    echo ❌ 测试数据生成器编译失败
) else (
    echo ✅ 测试数据生成器编译成功
)

echo.
echo 编译性能测试程序...
"%MSYS_PATH%" -c "cd '%PROJECT_PATH%' && gcc -O3 -std=c99 -I../../src performance_test.c ../../src/mont_field.c ../../src/mont_field_optimized.c ../../src/traditional_mul.c -o performance_test.exe"
if %errorlevel% neq 0 (
    echo ❌ 性能测试程序编译失败
) else (
    echo ✅ 性能测试程序编译成功
)

echo.
echo ==============================================
echo 运行内置测试数据
echo ==============================================
echo.

echo 🔐 运行内置测试数据收集器...
"%MSYS_PATH%" -c "cd '%PROJECT_PATH%' && ./test_data_collector.exe"
if %errorlevel% neq 0 (
    echo ❌ 内置测试数据收集失败
) else (
    echo ✅ 内置测试数据收集成功
)

echo.
echo 📊 运行内置测试数据生成器...
"%MSYS_PATH%" -c "cd '%PROJECT_PATH%' && ./test_data_generator.exe"
if %errorlevel% neq 0 (
    echo ❌ 内置测试数据生成失败
) else (
    echo ✅ 内置测试数据生成成功
)

echo.
echo ⚡ 运行内置性能测试...
"%MSYS_PATH%" -c "cd '%PROJECT_PATH%' && ./performance_test.exe"
if %errorlevel% neq 0 (
    echo ❌ 内置性能测试失败
) else (
    echo ✅ 内置性能测试成功
)

echo.
echo ==============================================
echo 内置测试数据运行完成！
echo ==============================================
echo.

echo 📊 查看生成的内置测试数据...
"%MSYS_PATH%" -c "cd '%PROJECT_PATH%' && ls -la *.txt *.data 2>/dev/null || echo '查看所有生成的文件:' && ls -la | grep -E '\.(txt|data|csv)$'"

echo.
echo 🎯 内置测试数据说明:
echo - 这些测试数据是项目中内置的
echo - 不需要额外的文件夹或外部数据
echo - 可以直接验证您的CSIDH-256优化算法
echo - 包含性能对比和正确性验证
echo.

echo 💡 优势:
echo - ✅ 简单易用，无需额外配置
echo - ✅ 测试数据稳定可靠
echo - ✅ 可以快速验证算法性能
echo - ✅ 适合演示和基本测试
echo.

pause


