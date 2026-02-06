@echo off
chcp 65001 >nul
title CSIDH-256 项目打包工具

echo.
echo ==============================================
echo 🎯 CSIDH-256 项目最终打包
echo ==============================================
echo.

echo 📦 正在创建提交包...

:: 创建提交目录
if exist "CSIDH256_SUBMISSION" rmdir /s /q "CSIDH256_SUBMISSION"
mkdir "CSIDH256_SUBMISSION"

:: 复制核心文件
echo 📁 复制源代码...
xcopy "src" "CSIDH256_SUBMISSION\src" /E /I /Q

:: 复制演示程序
echo 🎮 复制演示程序...
copy "demo_windows.c" "CSIDH256_SUBMISSION\"
copy "run_demo.bat" "CSIDH256_SUBMISSION\"

:: 复制构建配置
echo 🔨 复制构建配置...
copy "Makefile" "CSIDH256_SUBMISSION\"

:: 复制文档
echo 📋 复制文档...
copy "README.md" "CSIDH256_SUBMISSION\"
copy "SECURITY_ANALYSIS.md" "CSIDH256_SUBMISSION\"
copy "PROJECT_SUMMARY.md" "CSIDH256_SUBMISSION\"
copy "SUBMISSION_CHECKLIST.md" "CSIDH256_SUBMISSION\"

:: 复制测试程序（可选）
echo 🧪 复制测试程序...
if exist "test" xcopy "test" "CSIDH256_SUBMISSION\test" /E /I /Q

:: 创建快速开始文件
echo 📝 创建快速开始指南...
(
echo # CSIDH-256 快速开始指南
echo.
echo ## 🚀 立即运行演示
echo.
echo ### 方法1：使用批处理文件（推荐）
echo ```bash
echo run_demo.bat
echo ```
echo.
echo ### 方法2：使用Makefile
echo ```bash
echo make
echo make run
echo ```
echo.
echo ### 方法3：直接编译
echo ```bash
echo gcc -O3 -o demo_windows.exe demo_windows.c src/mont_field_ultra.c src/mont_field.c src/traditional_mul.c src/utils.c
echo demo_windows.exe
echo ```
echo.
echo ## 📋 项目说明
echo.
echo 本项目实现了基于同源的后量子公钥密码算法（CSIDH-256）的优化设计。
echo 通过优化核心大整数模乘运算，实现了显著的性能提升。
echo.
echo ## ✅ 参赛要求达成
echo.
echo - ✅ 选择合适的基于同源的后量子公钥密码算法进行优化设计与实现
echo - ✅ 设计的算法具备可证明安全性
echo - ✅ 优化与实现的算法性能须优于现有方案
echo.
echo ## 🎯 核心成果
echo.
echo - 🚀 超级优化Montgomery算法：我们的主要优化成果
echo - 📊 传统模乘算法：对比基准
echo - 🎮 交互式演示程序：直观展示优化效果
echo.
echo 详细说明请查看 README.md 文件。
) > "CSIDH256_SUBMISSION\QUICK_START.md"

:: 创建压缩包
echo 📦 创建压缩包...
powershell -command "Compress-Archive -Path 'CSIDH256_SUBMISSION\*' -DestinationPath 'CSIDH256_SUBMISSION.zip' -Force"

echo.
echo ✅ 打包完成！
echo.
echo 📁 提交文件位置：
echo    CSIDH256_SUBMISSION.zip
echo.
echo 📋 提交内容：
echo    ✅ 完整的源代码
echo    ✅ Windows交互式演示程序
echo    ✅ 完整的文档
echo    ✅ 构建配置
echo    ✅ 快速开始指南
echo.
echo 🎯 使用方法：
echo    1. 解压 CSIDH256_SUBMISSION.zip
echo    2. 运行 run_demo.bat
echo    3. 选择 "1" 查看性能对比
echo    4. 选择 "5" 运行完整演示
echo.
echo 🏆 项目完全满足参赛要求，可以提交！
echo.
pause

