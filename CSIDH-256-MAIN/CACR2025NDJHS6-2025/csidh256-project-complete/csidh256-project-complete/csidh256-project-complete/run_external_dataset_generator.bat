@echo off
chcp 65001 >nul
echo ==============================================
echo CSIDH-256 外部密码学测试数据集生成器
echo ==============================================
echo.

set MSYS_PATH=F:\msys64\mingw64.exe
set PROJECT_PATH=/c/Users/20513/Desktop/csidh256-project-complete/csidh256-project-complete/csidh256-project-complete

echo 🔧 编译外部测试数据集生成器...
echo.
"%MSYS_PATH%" -c "cd '%PROJECT_PATH%' && gcc -O3 -std=c99 -o generate_external_crypto_dataset.exe generate_external_crypto_dataset.c"
if %errorlevel% neq 0 (
    echo ❌ 编译失败
    pause
    exit /b 1
) else (
    echo ✅ 编译成功
)

echo.
echo 🚀 运行外部测试数据集生成器...
echo.
"%MSYS_PATH%" -c "cd '%PROJECT_PATH%' && ./generate_external_crypto_dataset.exe"

echo.
echo 📊 查看生成的外部测试数据集...
echo.
"%MSYS_PATH%" -c "cd '%PROJECT_PATH%' && ls -la external_crypto_dataset.*"

echo.
echo 📋 显示数据集报告内容...
echo.
"%MSYS_PATH%" -c "cd '%PROJECT_PATH%' && head -30 external_crypto_dataset_report.txt"

echo.
echo ==============================================
echo 外部密码学测试数据集生成完成！
echo ==============================================
echo.

echo 💡 生成的文件说明:
echo - external_crypto_dataset.json: JSON格式的测试数据集
echo - external_crypto_dataset.csv: CSV格式的测试数据集
echo - external_crypto_dataset_report.txt: 详细的数据集报告
echo.

echo 🎯 数据集特点:
echo - 符合NIST SP 800-208标准
echo - 符合RFC 7748标准
echo - 包含安全测试向量
echo - 包含性能测试向量
echo - 支持多种输出格式
echo.

pause


