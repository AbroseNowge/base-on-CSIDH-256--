@echo off
chcp 65001 >nul
echo ==============================================
echo CSIDH-256 密码学测试工具集运行脚本
echo ==============================================
echo.

:: 检查编译器是否可用
where gcc >nul 2>&1
if %errorlevel% neq 0 (
    echo ❌ 错误：未找到GCC编译器！
    echo 请确保已安装MSYS2/MINGW64并添加到PATH环境变量
    pause
    exit /b 1
)

echo ✅ 编译器检查通过
echo.

:: 创建输出目录
if not exist "crypto_test_output" mkdir crypto_test_output
cd crypto_test_output

echo 📁 当前工作目录: %CD%
echo.

:: 编译密码学测试向量生成器
echo 🔧 编译密码学测试向量生成器...
gcc -O3 -Wall -Wextra -std=c99 -I../src ../crypto_test_vectors_generator.c ../src/mont_field.c ../src/mont_field_optimized.c ../src/traditional_mul.c -o crypto_test_vectors_generator.exe
if %errorlevel% neq 0 (
    echo ❌ 密码学测试向量生成器编译失败！
    pause
    exit /b 1
)
echo ✅ 密码学测试向量生成器编译成功

:: 编译增强版密码学测试收集器
echo 🔧 编译增强版密码学测试收集器...
gcc -O3 -Wall -Wextra -std=c99 -I../src ../enhanced_crypto_test_collector.c ../src/mont_field.c ../src/mont_field_optimized.c ../src/traditional_mul.c -o enhanced_crypto_test_collector.exe
if %errorlevel% neq 0 (
    echo ❌ 增强版密码学测试收集器编译失败！
    pause
    exit /b 1
)
echo ✅ 增强版密码学测试收集器编译成功

:: 编译密码学性能测试数据生成器
echo 🔧 编译密码学性能测试数据生成器...
gcc -O3 -Wall -Wextra -std=c99 -I../src ../crypto_performance_test_data_generator.c ../src/mont_field.c ../src/mont_field_optimized.c ../src/traditional_mul.c -o crypto_performance_test_data_generator.exe
if %errorlevel% neq 0 (
    echo ❌ 密码学性能测试数据生成器编译失败！
    pause
    exit /b 1
)
echo ✅ 密码学性能测试数据生成器编译成功

echo.
echo ==============================================
echo 所有密码学测试工具编译完成！
echo ==============================================
echo.

:: 运行测试工具菜单
:menu
echo 请选择要运行的测试工具：
echo.
echo 1. 密码学测试向量生成器
echo 2. 增强版密码学测试收集器
echo 3. 密码学性能测试数据生成器
echo 4. 运行所有测试工具
echo 5. 查看生成的文件
echo 0. 退出
echo.
set /p choice="请输入选择 (0-5): "

if "%choice%"=="1" goto run_vectors_generator
if "%choice%"=="2" goto run_enhanced_collector
if "%choice%"=="3" goto run_performance_generator
if "%choice%"=="4" goto run_all_tools
if "%choice%"=="5" goto list_files
if "%choice%"=="0" goto end
echo ❌ 无效选择，请重新输入
goto menu

:run_vectors_generator
echo.
echo 🔐 运行密码学测试向量生成器...
echo.
crypto_test_vectors_generator.exe
echo.
echo ✅ 密码学测试向量生成器运行完成
pause
goto menu

:run_enhanced_collector
echo.
echo 🚀 运行增强版密码学测试收集器...
echo.
enhanced_crypto_test_collector.exe
echo.
echo ✅ 增强版密码学测试收集器运行完成
pause
goto menu

:run_performance_generator
echo.
echo ⚡ 运行密码学性能测试数据生成器...
echo.
crypto_performance_test_data_generator.exe
echo.
echo ✅ 密码学性能测试数据生成器运行完成
pause
goto menu

:run_all_tools
echo.
echo 🎯 运行所有密码学测试工具...
echo.

echo 🔐 步骤1: 生成密码学测试向量...
echo.
crypto_test_vectors_generator.exe
echo.

echo 🚀 步骤2: 运行增强版密码学测试收集器...
echo.
enhanced_crypto_test_collector.exe
echo.

echo ⚡ 步骤3: 生成密码学性能测试数据...
echo.
crypto_performance_test_data_generator.exe
echo.

echo ✅ 所有密码学测试工具运行完成！
echo.
echo 📊 测试数据生成汇总：
echo - 密码学测试向量 (JSON/CSV格式)
echo - 增强版性能测试结果
echo - 密码学验证结果
echo - 性能基准测试数据
echo - 详细的测试报告
echo.
pause
goto menu

:list_files
echo.
echo 📁 当前目录中的测试文件：
echo.
dir /b *.json *.csv *.txt *.log 2>nul
if %errorlevel% neq 0 (
    echo 暂无测试文件生成
)
echo.
pause
goto menu

:end
echo.
echo 👋 感谢使用CSIDH-256密码学测试工具集！
echo.
cd ..
pause


