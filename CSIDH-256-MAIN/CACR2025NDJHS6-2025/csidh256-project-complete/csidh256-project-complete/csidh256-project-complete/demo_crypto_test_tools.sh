#!/bin/bash

echo "=============================================="
echo "CSIDH-256 密码学测试工具集演示 (Bash版本)"
echo "=============================================="
echo

echo "🎯 欢迎使用CSIDH-256密码学测试工具集！"
echo
echo "本工具集专为后量子密码优化方案设计，提供："
echo "- 标准化的密码学测试向量"
echo "- 详细的性能基准测试"
echo "- 安全验证和威胁分析"
echo "- 多种输出格式支持"
echo

# 检查编译器
if ! command -v gcc &> /dev/null; then
    echo "❌ 错误：未找到GCC编译器！"
    echo "请确保已安装MSYS2/MINGW64"
    read -p "按回车键继续..."
    exit 1
fi

echo "✅ 编译器检查通过"
echo

# 创建演示输出目录
mkdir -p "crypto_demo_output"
cd crypto_demo_output

echo "📁 演示输出目录: $(pwd)"
echo

# 编译演示工具
echo "🔧 编译演示工具..."
gcc -O3 -Wall -Wextra -std=c99 -I../src ../csidh256-project-complete/crypto_test_vectors_generator.c ../src/mont_field.c ../src/mont_field_optimized.c ../src/traditional_mul.c -o demo_vectors_generator.exe
if [ $? -ne 0 ]; then
    echo "❌ 编译失败！"
    read -p "按回车键继续..."
    exit 1
fi

gcc -O3 -Wall -Wextra -std=c99 -I../src ../csidh256-project-complete/enhanced_crypto_test_collector.c ../src/mont_field.c ../src/mont_field_optimized.c ../src/traditional_mul.c -o demo_enhanced_collector.exe
if [ $? -ne 0 ]; then
    echo "❌ 编译失败！"
    read -p "按回车键继续..."
    exit 1
fi

gcc -O3 -Wall -Wextra -std=c99 -I../src ../csidh256-project-complete/crypto_test_data_validator.c ../src/mont_field.c ../src/mont_field_optimized.c ../src/traditional_mul.c -o demo_validator.exe
if [ $? -ne 0 ]; then
    echo "❌ 编译失败！"
    read -p "按回车键继续..."
    exit 1
fi

echo "✅ 演示工具编译成功"
echo

echo "=============================================="
echo "开始密码学测试工具演示"
echo "=============================================="
echo

echo "🔐 步骤1: 生成密码学测试向量"
echo "----------------------------------------------"
echo "正在生成NIST标准测试向量..."
echo "1" | ./demo_vectors_generator.exe >/dev/null 2>&1
if [ -f "nist_test_vectors.json" ]; then
    echo "✅ NIST标准测试向量生成成功"
    echo "📁 文件: nist_test_vectors.json"
else
    echo "❌ NIST标准测试向量生成失败"
fi
echo

echo "🛡️ 步骤2: 生成安全测试向量"
echo "----------------------------------------------"
echo "正在生成安全测试向量..."
echo "4" | ./demo_vectors_generator.exe >/dev/null 2>&1
if [ -f "security_test_vectors.json" ]; then
    echo "✅ 安全测试向量生成成功"
    echo "📁 文件: security_test_vectors.json"
else
    echo "❌ 安全测试向量生成失败"
fi
echo

echo "⚡ 步骤3: 运行增强版性能测试"
echo "----------------------------------------------"
echo "正在运行性能测试..."
./demo_enhanced_collector.exe >/dev/null 2>&1
if [ -f "enhanced_crypto_test_results.json" ]; then
    echo "✅ 增强版性能测试完成"
    echo "📁 文件: enhanced_crypto_test_results.json"
else
    echo "❌ 性能测试失败"
fi
echo

echo "🔍 步骤4: 验证测试数据"
echo "----------------------------------------------"
echo "正在验证测试数据..."
echo "1" | ./demo_validator.exe >/dev/null 2>&1
if [ -f "crypto_validation_log.txt" ]; then
    echo "✅ 测试数据验证完成"
    echo "📁 文件: crypto_validation_log.txt"
else
    echo "❌ 测试数据验证失败"
fi
echo

echo "=============================================="
echo "演示完成 - 查看生成的文件"
echo "=============================================="
echo

echo "📊 生成的文件列表:"
echo "----------------------------------------------"
ls -la *.json *.csv *.txt *.log 2>/dev/null || echo "暂无文件生成"
echo

echo "📋 文件说明:"
echo "----------------------------------------------"
echo "- *.json  : 结构化测试数据 (JSON格式)"
echo "- *.csv   : 表格化测试数据 (CSV格式)"
echo "- *.txt   : 详细测试报告 (文本格式)"
echo "- *.log   : 测试执行日志 (日志格式)"
echo

echo "🎯 下一步操作建议:"
echo "----------------------------------------------"
echo "1. 查看生成的JSON文件了解测试数据结构"
echo "2. 使用Excel打开CSV文件进行数据分析"
echo "3. 阅读TXT报告文件了解详细测试结果"
echo "4. 检查LOG文件了解测试执行过程"
echo

echo "💡 提示:"
echo "----------------------------------------------"
echo "- 所有测试数据都基于CSIDH-256后量子密码算法"
echo "- 测试向量符合NIST和RFC标准"
echo "- 性能指标包含执行时间、吞吐量、能效比等"
echo "- 安全测试覆盖侧信道攻击、时序攻击等威胁模型"
echo

echo "🔗 相关文档:"
echo "----------------------------------------------"
echo "- CRYPTO_TEST_TOOLS_README.md : 详细使用说明"
echo "- 项目README.md : 项目总体介绍"
echo

echo "演示完成！感谢使用CSIDH-256密码学测试工具集！"
echo
read -p "按回车键继续..."
cd ..


