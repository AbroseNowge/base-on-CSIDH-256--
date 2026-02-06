# MSYS2 MINGW64 编译运行指南

本文档提供在 MSYS2 MINGW64 环境中编译和运行 CSIDH-256 性能测试程序的完整命令。

## 📋 环境准备

### 1. 确保已安装必要的工具

```bash
# 检查GCC版本
gcc --version

# 如果没有安装，使用以下命令安装
pacman -S mingw-w64-x86_64-gcc
pacman -S make
```

### 2. 切换到项目目录

```bash
cd /c/Users/20513/Desktop/CACR2025NDJHS6-2025/csidh256-project-complete/csidh256-project-complete
```

## 🔧 编译命令

### 方案1: 使用内置测试数据的性能测试程序

#### 编译 `performance_comparison_test.c`

```bash
gcc -O3 -Wall -Isrc \
    -o performance_comparison_test.exe \
    performance_comparison_test.c \
    src/mont_field.c \
    src/optimized_montgomery_algorithm.c \
    src/traditional_mul.c \
    src/utils.c \
    -lm
```

**单行命令（推荐）**：
```bash
gcc -O3 -Wall -Isrc -o performance_comparison_test.exe performance_comparison_test.c src/mont_field.c src/optimized_montgomery_algorithm.c src/traditional_mul.c src/utils.c -lm
```

#### 运行内置数据测试

```bash
./performance_comparison_test.exe
```

---

### 方案2: 支持外部测试数据的性能测试程序

#### 编译 `performance_test_with_external.c`

```bash
gcc -O3 -Wall -Isrc \
    -o performance_test_with_external.exe \
    performance_test_with_external.c \
    src/external_test_data.c \
    src/mont_field.c \
    src/optimized_montgomery_algorithm.c \
    src/traditional_mul.c \
    src/utils.c \
    -lm
```

**单行命令（推荐）**：
```bash
gcc -O3 -Wall -Isrc -o performance_test_with_external.exe performance_test_with_external.c src/external_test_data.c src/mont_field.c src/optimized_montgomery_algorithm.c src/traditional_mul.c src/utils.c -lm
```

#### 运行外部数据测试

**使用内置数据（默认）**：
```bash
./performance_test_with_external.exe
```

**使用外部测试数据文件**：
```bash
./performance_test_with_external.exe --external test_data_external.json
```

**使用外部数据的指定向量**：
```bash
./performance_test_with_external.exe --external test_data_external.json --vector 2
```

**列出外部数据中的所有向量**：
```bash
./performance_test_with_external.exe --external test_data_external.json --list
```

**自定义迭代次数**：
```bash
./performance_test_with_external.exe --external test_data_external.json --iterations 1000000
```

**显示帮助信息**：
```bash
./performance_test_with_external.exe --help
```

---

## 🚀 快速编译脚本

### 创建编译脚本

创建一个文件 `compile_all.sh`：

```bash
#!/bin/bash

echo "=========================================="
echo "CSIDH-256 编译脚本"
echo "=========================================="
echo ""

# 编译内置数据测试程序
echo "[1/2] 编译内置数据测试程序..."
gcc -O3 -Wall -Isrc -o performance_comparison_test.exe \
    performance_comparison_test.c \
    src/mont_field.c \
    src/optimized_montgomery_algorithm.c \
    src/traditional_mul.c \
    src/utils.c \
    -lm

if [ $? -eq 0 ]; then
    echo "✓ 编译成功: performance_comparison_test.exe"
else
    echo "✗ 编译失败"
    exit 1
fi

echo ""

# 编译外部数据测试程序
echo "[2/2] 编译外部数据测试程序..."
gcc -O3 -Wall -Isrc -o performance_test_with_external.exe \
    performance_test_with_external.c \
    src/external_test_data.c \
    src/mont_field.c \
    src/optimized_montgomery_algorithm.c \
    src/traditional_mul.c \
    src/utils.c \
    -lm

if [ $? -eq 0 ]; then
    echo "✓ 编译成功: performance_test_with_external.exe"
else
    echo "✗ 编译失败"
    exit 1
fi

echo ""
echo "=========================================="
echo "编译完成！"
echo "=========================================="
```

**使用编译脚本**：
```bash
chmod +x compile_all.sh
./compile_all.sh
```

---

## 📝 使用 Makefile（推荐）

### 编译所有程序

```bash
make
```

### 编译特定程序

```bash
# 编译内置数据测试程序
make performance_comparison_test.exe

# 编译外部数据测试程序
make performance_test_with_external.exe
```

### 运行测试

```bash
# 运行内置数据测试
make run-performance

# 运行外部数据测试
make run-performance-external
```

### 清理编译文件

```bash
make clean
```

---

## 🎯 完整示例流程

### 示例1: 使用内置数据测试

```bash
# 1. 切换到项目目录
cd /c/Users/20513/Desktop/CACR2025NDJHS6-2025/csidh256-project-complete/csidh256-project-complete

# 2. 编译
gcc -O3 -Wall -Isrc -o performance_comparison_test.exe \
    performance_comparison_test.c \
    src/mont_field.c \
    src/optimized_montgomery_algorithm.c \
    src/traditional_mul.c \
    src/utils.c -lm

# 3. 运行
./performance_comparison_test.exe
```

### 示例2: 使用外部数据测试

```bash
# 1. 切换到项目目录
cd /c/Users/20513/Desktop/CACR2025NDJHS6-2025/csidh256-project-complete/csidh256-project-complete

# 2. 编译
gcc -O3 -Wall -Isrc -o performance_test_with_external.exe \
    performance_test_with_external.c \
    src/external_test_data.c \
    src/mont_field.c \
    src/optimized_montgomery_algorithm.c \
    src/traditional_mul.c \
    src/utils.c -lm

# 3. 查看外部数据中的测试向量
./performance_test_with_external.exe --external test_data_external.json --list

# 4. 使用外部数据运行测试
./performance_test_with_external.exe --external test_data_external.json

# 5. 使用指定向量和自定义迭代次数
./performance_test_with_external.exe --external test_data_external.json --vector 1 --iterations 1000000
```

---

## 🔍 编译选项说明

### 常用编译选项

- `-O3`: 最高级别优化
- `-Wall`: 启用所有警告
- `-Isrc`: 指定头文件搜索路径
- `-o <文件名>`: 指定输出文件名
- `-lm`: 链接数学库

### 高级编译选项（可选）

```bash
# 添加调试信息
gcc -O3 -Wall -g -Isrc ...

# 添加架构优化
gcc -O3 -Wall -march=native -mtune=native -Isrc ...

# 启用OpenMP（如果使用）
gcc -O3 -Wall -fopenmp -Isrc ...
```

---

## ⚠️ 常见问题

### 问题1: 找不到头文件

**错误信息**：
```
fatal error: src/mont_field.h: No such file or directory
```

**解决方法**：
- 确保在项目根目录执行编译命令
- 检查 `-Isrc` 选项是否正确
- 验证文件路径是否正确

### 问题2: 链接错误

**错误信息**：
```
undefined reference to `mont_field_init_optimized'
```

**解决方法**：
- 确保包含了所有必需的源文件
- 检查源文件路径是否正确
- 确保函数名拼写正确

### 问题3: 找不到外部数据文件

**错误信息**：
```
错误: 无法打开文件: test_data_external.json
```

**解决方法**：
- 确保 `test_data_external.json` 文件存在于当前目录
- 使用绝对路径或相对路径指定文件位置
- 检查文件权限

---

## 📊 性能对比

编译完成后，可以对比两种测试方式：

| 测试方式 | 程序文件 | 数据来源 | 灵活性 |
|---------|---------|---------|--------|
| 内置数据 | `performance_comparison_test.exe` | 代码中硬编码 | 低 |
| 外部数据 | `performance_test_with_external.exe` | JSON文件 | 高 |

---

## 🎓 总结

### 快速参考

**编译内置数据测试**：
```bash
gcc -O3 -Wall -Isrc -o performance_comparison_test.exe performance_comparison_test.c src/mont_field.c src/optimized_montgomery_algorithm.c src/traditional_mul.c src/utils.c -lm
```

**编译外部数据测试**：
```bash
gcc -O3 -Wall -Isrc -o performance_test_with_external.exe performance_test_with_external.c src/external_test_data.c src/mont_field.c src/optimized_montgomery_algorithm.c src/traditional_mul.c src/utils.c -lm
```

**运行测试**：
```bash
# 内置数据
./performance_comparison_test.exe

# 外部数据（默认使用内置）
./performance_test_with_external.exe

# 外部数据（使用JSON文件）
./performance_test_with_external.exe --external test_data_external.json
```

---

**文档版本**: 1.0  
**最后更新**: 2025年  
**维护者**: CSIDH-256 项目组


