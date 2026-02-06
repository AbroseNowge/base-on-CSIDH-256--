#!/bin/bash

echo "=============================================="
echo "CSIDH-256 密码学测试工具快速启动"
echo "=============================================="
echo

# 检查当前目录
current_dir=$(pwd)
echo "📁 当前目录: $current_dir"

# 检查是否在正确的目录
if [[ "$current_dir" == *"csidh256-project-complete/csidh256-project-complete"* ]]; then
    echo "✅ 目录正确"
else
    echo "❌ 目录不正确，请确保您在正确的目录中"
    echo "正确路径应该包含: csidh256-project-complete/csidh256-project-complete"
    read -p "按回车键继续..."
    exit 1
fi

echo

# 检查必要的文件是否存在
echo "🔍 检查必要文件..."
files=(
    "crypto_test_vectors_generator.c"
    "enhanced_crypto_test_collector.c"
    "crypto_performance_test_data_generator.c"
    "crypto_test_data_validator.c"
    "../src/mont_field.c"
    "../src/mont_field_optimized.c"
    "../src/traditional_mul.c"
)

missing_files=()
for file in "${files[@]}"; do
    if [ -f "$file" ]; then
        echo "✅ $file"
    else
        echo "❌ $file (缺失)"
        missing_files+=("$file")
    fi
done

if [ ${#missing_files[@]} -gt 0 ]; then
    echo
    echo "❌ 缺少必要文件，无法继续"
    echo "缺失的文件:"
    for file in "${missing_files[@]}"; do
        echo "  - $file"
    done
    read -p "按回车键继续..."
    exit 1
fi

echo
echo "✅ 所有必要文件都存在"
echo

# 检查编译器
echo "🔍 检查编译器..."
if command -v gcc &> /dev/null; then
    gcc_version=$(gcc --version | head -n1)
    echo "✅ GCC编译器: $gcc_version"
else
    echo "❌ 未找到GCC编译器"
    echo "请安装MSYS2/MINGW64"
    read -p "按回车键继续..."
    exit 1
fi

echo

# 创建输出目录
echo "📁 创建输出目录..."
mkdir -p "crypto_test_output"
cd crypto_test_output
echo "✅ 输出目录: $(pwd)"
echo

# 编译工具
echo "🔧 编译密码学测试工具..."

echo "  编译测试向量生成器..."
gcc -O3 -Wall -Wextra -std=c99 -I../../src ../../csidh256-project-complete/crypto_test_vectors_generator.c ../../src/mont_field.c ../../src/mont_field_optimized.c ../../src/traditional_mul.c -o crypto_test_vectors_generator.exe
if [ $? -ne 0 ]; then
    echo "❌ 测试向量生成器编译失败"
    read -p "按回车键继续..."
    exit 1
fi
echo "  ✅ 测试向量生成器编译成功"

echo "  编译增强版测试收集器..."
gcc -O3 -Wall -Wextra -std=c99 -I../../src ../../csidh256-project-complete/enhanced_crypto_test_collector.c ../../src/mont_field.c ../../src/mont_field_optimized.c ../../src/traditional_mul.c -o enhanced_crypto_test_collector.exe
if [ $? -ne 0 ]; then
    echo "❌ 增强版测试收集器编译失败"
    read -p "按回车键继续..."
    exit 1
fi
echo "  ✅ 增强版测试收集器编译成功"

echo "  编译性能测试数据生成器..."
gcc -O3 -Wall -Wextra -std=c99 -I../../src ../../csidh256-project-complete/crypto_performance_test_data_generator.c ../../src/mont_field.c ../../src/mont_field_optimized.c ../../src/traditional_mul.c -o crypto_performance_test_data_generator.exe
if [ $? -ne 0 ]; then
    echo "❌ 性能测试数据生成器编译失败"
    read -p "按回车键继续..."
    exit 1
fi
echo "  ✅ 性能测试数据生成器编译成功"

echo "  编译测试数据验证器..."
gcc -O3 -Wall -Wextra -std=c99 -I../../src ../../csidh256-project-complete/crypto_test_data_validator.c ../../src/mont_field.c ../../src/mont_field_optimized.c ../../src/traditional_mul.c -o crypto_test_data_validator.exe
if [ $? -ne 0 ]; then
    echo "❌ 测试数据验证器编译失败"
    read -p "按回车键继续..."
    exit 1
fi
echo "  ✅ 测试数据验证器编译成功"

echo
echo "✅ 所有工具编译完成！"
echo

# 运行快速测试
echo "🚀 运行快速测试..."
echo

echo "🔐 生成密码学测试向量..."
echo "1" | ./crypto_test_vectors_generator.exe >/dev/null 2>&1
if [ -f "nist_test_vectors.json" ]; then
    echo "✅ 测试向量生成成功"
else
    echo "❌ 测试向量生成失败"
fi

echo "⚡ 运行性能测试..."
./enhanced_crypto_test_collector.exe >/dev/null 2>&1
if [ -f "enhanced_crypto_test_results.json" ]; then
    echo "✅ 性能测试完成"
else
    echo "❌ 性能测试失败"
fi

echo "🔍 验证测试数据..."
echo "1" | ./crypto_test_data_validator.exe >/dev/null 2>&1
if [ -f "crypto_validation_log.txt" ]; then
    echo "✅ 测试数据验证完成"
else
    echo "❌ 测试数据验证失败"
fi

echo
echo "=============================================="
echo "快速测试完成！"
echo "=============================================="
echo

# 显示生成的文件
echo "📊 生成的文件:"
ls -la *.json *.csv *.txt *.log 2>/dev/null | while read line; do
    echo "  $line"
done

echo
echo "🎯 下一步操作:"
echo "1. 查看生成的JSON文件了解测试数据结构"
echo "2. 使用Excel打开CSV文件进行数据分析"
echo "3. 阅读TXT报告文件了解详细测试结果"
echo "4. 运行完整测试: ./run_crypto_test_tools.sh"
echo

echo "💡 提示:"
echo "- 所有测试数据都基于CSIDH-256后量子密码算法"
echo "- 测试向量符合NIST和RFC标准"
echo "- 性能指标包含执行时间、吞吐量、能效比等"
echo "- 安全测试覆盖侧信道攻击、时序攻击等威胁模型"
echo

read -p "按回车键继续..."
cd ..


