# CSIDH-256 后量子密码算法优化项目

## 📋 项目简介

本项目实现了CSIDH-256算法的优化设计，主要对比传统模乘算法和优化Montgomery算法的性能差异。通过Montgomery算法的优化实现，成功实现了50%以上的性能提升。项目提供了完整的交互式演示、性能测试工具，并支持外部测试数据和交互式自定义数据输入。

## 🎯 项目特点

- **算法优化**: 基于Montgomery算法的优化实现
- **性能提升**: 相比传统算法提升50%以上（基于实际测试结果动态计算）
- **安全性保证**: 完全保持原有的安全特性
- **易于使用**: 提供交互式演示和自动化测试
- **外部数据支持**: 支持使用外部JSON测试数据文件，提高测试灵活性
- **交互式输入**: 支持在交互页面直接输入自定义测试数据，无需修改文件
- **动态性能分析**: 性能对比结果基于实际测试数据实时计算

## 📁 项目结构

```
csidh256-project-complete/
├── src/                                    # 源代码目录
│   ├── mont_field.h                       # Montgomery算法头文件
│   ├── mont_field.c                       # 基础Montgomery算法实现
│   ├── optimized_montgomery_algorithm.c   # 优化Montgomery算法实现
│   ├── traditional_mul.h                  # 传统算法头文件
│   ├── traditional_mul.c                  # 传统模乘算法实现
│   ├── params.h                           # CSIDH-256参数定义
│   ├── external_test_data.h               # 外部测试数据加载头文件
│   ├── external_test_data.c               # 外部测试数据加载实现
│   └── utils.c                            # 工具函数
├── demo_windows.c                         # 交互式演示程序（主程序）
├── performance_comparison_test.c           # 内部数据性能对比测试程序
├── performance_test_with_external.c        # 支持外部数据的性能测试程序
├── test_data_external.json                 # 示例外部测试数据文件
├── run_external_test.bat                   # 运行外部数据测试批处理文件
├── 外部测试数据使用说明.md                 # 外部测试数据使用文档
├── 外部数据调用逻辑说明.md                 # 外部数据调用逻辑说明
├── 测评原理.md                             # 性能测评原理文档
├── MSYS2编译运行指南.md                    # MSYS2编译运行详细指南
├── Makefile                               # 编译配置文件
└── README.md                              # 项目说明文档（本文件）
```

## 🚀 快速开始

### 环境要求

- **操作系统**: Windows 10/11
- **编译器**: GCC (推荐使用MSYS2/MinGW-w64)
- **内存**: 至少4GB RAM
- **存储**: 至少100MB可用空间

### 最简单的运行方法

1. **打开MSYS2 MINGW64终端**
2. **切换到项目目录**:
   ```bash
   cd /c/Users/20513/Desktop/CACR2025NDJHS6-2025/csidh256-project-complete/csidh256-project-complete
   ```
3. **直接运行演示程序**:
   ```bash
   ./demo_windows.exe
   ```

就这么简单！程序会显示交互式菜单，你可以选择不同的功能进行测试。

### 安装MSYS2 (如果还没有)

1. 下载MSYS2: https://www.msys2.org/
2. 安装MSYS2到默认目录
3. 打开MSYS2 MINGW64终端
4. 安装GCC编译器:
   ```bash
   pacman -S mingw-w64-x86_64-gcc
   ```

5.运行方式1.直接运行demo_windows.exe文件进行交互
6.运行方式2：
点入F盘
点入MSY,点入mingw64.exe,
运行 cd "C:\Users\20513\Desktop\CACR2025NDJHS6-2025\csidh256-project-complete\csidh256-project-complete"
进入后，输入./demo_windows.exe进行运行(有下划线，被挡住无法显示）
如果要单独展示传统数据交互页面，输入./performance_comparison_test.exe(有下划线）
如果要单独展示外部数据交互页面，输入 ./performance_test_with_external.exe --external test_data_external.json(有下划线）
如果进行了代码重新编辑，需要进行重新配置：
make demo_windows.exe
make ./performance_comparison_test.exe
make ./performance_test_with_external.exe
或
手动编译：
gcc -O3 -Isrc -o demo_windows.exe demo_windows.c src/mont_field_simple_optimized.c src/mont_field.c src/traditional_mul.c src/utils.c -lm
gcc -O3 -Wall -Isrc -o performance_test_with_external.exe \
    performance_test_with_external.c \
    src/external_test_data.c \
    src/mont_field.c \
    src/optimized_montgomery_algorithm.c \
    src/traditional_mul.c \
    src/utils.c -lm

## 📖 详细操作指南

### 方法1: 交互式演示程序（推荐）

#### 🎮 运行交互式演示程序

**步骤**:
1. 打开MSYS2 MINGW64终端
2. 切换到项目目录:
   ```bash
   cd /c/Users/20513/Desktop/CACR2025NDJHS6-2025/csidh256-project-complete/csidh256-project-complete
   ```
3. 运行交互式演示:
   ```bash
   # 直接运行现有的可执行文件（推荐）
   ./demo_windows.exe
   
   # 或者重新编译并运行
   gcc -O3 -Isrc -o demo_windows.exe demo_windows.c src/mont_field_simple_optimized.c src/mont_field.c src/traditional_mul.c src/utils.c -lm
   ./demo_windows.exe
   ```

**完整功能菜单**:
```
🎮 交互式演示菜单
==============================================

请选择操作:
1. 📊 运行性能对比测试
2. ℹ️  显示项目信息
3. 🔍 显示算法技术细节
4. 🛡️  显示安全性分析
5. 🚀 运行完整演示
6. 📈 内部数据性能测试（详细报告）
7. 📊 外部数据性能测试（支持自定义数据）
0. 🚪 退出程序

请输入选择 (0-7):
```

#### 菜单选项详细说明

**选项 1: 📊 运行性能对比测试**
- 快速性能对比测试
- 使用内置测试数据
- 显示性能提升效果

**选项 2: ℹ️ 显示项目信息**
- 项目概述和特点
- 优化技术说明
- 参赛要求达成情况

**选项 3: 🔍 显示算法技术细节**
- Montgomery算法技术细节
- 传统算法对比
- 优化技术说明

**选项 4: 🛡️ 显示安全性分析**
- CSIDH-256安全性基础
- 优化安全性保证
- 性能与安全平衡

**选项 5: 🚀 运行完整演示**
- 完整的项目演示
- 包含所有功能展示
- 性能对比结果

**选项 6: 📈 内部数据性能测试（详细报告）**
- 调用 `performance_comparison_test.exe`
- 使用内置测试数据
- 生成详细的性能报告
- **性能对比结果基于实际测试数据动态计算**

**选项 7: 📊 外部数据性能测试（支持自定义数据）**
- 支持两种数据来源：
  - **方式1**: 使用JSON文件 (`test_data_external.json`)
  - **方式2**: ✏️ **交互式输入自定义数据**（推荐）
- 交互式输入功能：
  - 直接在交互页面输入测试数据
  - 无需修改JSON文件
  - 实时输入，立即测试
  - 支持自定义迭代次数

#### 交互式自定义数据输入（选项7 → 选项2）

选择选项7后，再选择选项2，可以交互式输入测试数据：

```
请输入 测试数据A (256位，需要4个64位十六进制数):
格式: 每个值可以是 0x123456789ABCDEF0 或 123456789ABCDEF0
提示: 输入4个值，用空格或换行分隔

  Limb[0] (64位十六进制): 0x123456789ABCDEF0
  ✓ 已输入: 0x123456789ABCDEF0
  Limb[1] (64位十六进制): 0xFEDCBA9876543210
  ✓ 已输入: 0xFEDCBA9876543210
  ...
```

**特点**:
- ✅ 实时输入验证
- ✅ 错误提示和重试
- ✅ 支持带/不带0x前缀
- ✅ 自动运行性能测试
- ✅ 显示详细性能对比结果

### 方法2: 使用Makefile编译

#### 编译所有程序
```bash
make
```

#### 编译特定程序
```bash
# 编译内部数据测试程序
make performance_comparison_test.exe

# 编译外部数据测试程序
make performance_test_with_external.exe

# 编译交互式演示程序
gcc -O3 -Isrc -o demo_windows.exe demo_windows.c src/mont_field_simple_optimized.c src/mont_field.c src/traditional_mul.c src/utils.c -lm
```

#### 运行测试
```bash
# 运行内置数据测试
make run-performance

# 运行外部数据测试
make run-performance-external
```

#### 清理编译文件
```bash
make clean
```

### 方法3: 手动编译（MSYS2 MINGW64）

#### 编译交互式演示程序（主程序）
```bash
gcc -O3 -Wall -Isrc -o demo_windows.exe \
    demo_windows.c \
    src/mont_field_simple_optimized.c \
    src/mont_field.c \
    src/traditional_mul.c \
    src/utils.c -lm
```

#### 编译内部数据性能测试程序
```bash
gcc -O3 -Wall -Isrc -o performance_comparison_test.exe \
    performance_comparison_test.c \
    src/mont_field.c \
    src/optimized_montgomery_algorithm.c \
    src/traditional_mul.c \
    src/utils.c -lm
```

#### 编译外部数据性能测试程序
```bash
gcc -O3 -Wall -Isrc -o performance_test_with_external.exe \
    performance_test_with_external.c \
    src/external_test_data.c \
    src/mont_field.c \
    src/optimized_montgomery_algorithm.c \
    src/traditional_mul.c \
    src/utils.c -lm
```

## 🔄 外部测试数据功能

### 功能概述

项目支持两种方式使用自定义测试数据：

1. **JSON文件方式**: 从JSON文件加载测试数据
2. **交互式输入方式**: 在交互页面直接输入测试数据（推荐）

### 方式1: 使用JSON文件

#### 快速使用

**使用内置数据（默认）**:
```bash
./performance_test_with_external.exe
```

**使用外部测试数据文件**:
```bash
./performance_test_with_external.exe --external test_data_external.json
```

**使用指定测试向量**:
```bash
./performance_test_with_external.exe --external test_data_external.json --vector 2
```

**列出所有测试向量**:
```bash
./performance_test_with_external.exe --external test_data_external.json --list
```

**自定义迭代次数**:
```bash
./performance_test_with_external.exe --external test_data_external.json --iterations 1000000
```

#### JSON文件格式

外部测试数据文件必须是有效的JSON格式：

```json
{
  "test_vectors": [
    {
      "test_name": "测试向量名称",
      "description": "测试向量描述",
      "category": "STANDARD|EDGE|SECURITY|PERFORMANCE|NIST",
      "input_a": {
        "limbs": [
          "0x123456789ABCDEF0",
          "0xFEDCBA9876543210",
          "0x0011223344556677",
          "0x1899AABBCCDDEEFF"
        ]
      },
      "input_b": {
        "limbs": [
          "0xAAAAAAAAAAAAAAAA",
          "0x5555555555555555",
          "0xCCCCCCCCCCCCCCCC",
          "0x1333333333333333"
        ]
      }
    }
  ]
}
```

### 方式2: 交互式输入（推荐）

**使用步骤**:
1. 运行 `./demo_windows.exe`
2. 选择选项 **7**（外部数据性能测试）
3. 选择选项 **2**（交互式输入自定义数据）
4. 按照提示输入测试数据A和B
5. 输入迭代次数（可选，默认100000）
6. 自动运行性能测试并显示结果

**优势**:
- ✅ 无需修改文件
- ✅ 实时输入验证
- ✅ 立即看到结果
- ✅ 适合快速测试

### 详细文档

更多关于外部测试数据的使用方法，请参阅：
- 📄 [外部测试数据使用说明.md](外部测试数据使用说明.md) - 完整的使用文档
- 📄 [外部数据调用逻辑说明.md](外部数据调用逻辑说明.md) - 技术实现说明
- 📄 [测评原理.md](测评原理.md) - 性能测评原理说明
- 📄 [MSYS2编译运行指南.md](MSYS2编译运行指南.md) - 编译运行详细指南

## 📊 性能测试结果

### 动态性能计算

**重要**: 性能对比结果现在基于**实际测试数据动态计算**，不再是固定的硬编码值。每次运行会根据实际测试结果计算：

- 相对性能倍数
- 性能提升百分比
- 每次操作节省的时间

### 典型性能数据示例

| 算法版本 | 每次操作耗时 | 吞吐量 | 相对性能 | 优化程度 |
|---------|-------------|--------|---------|---------|
| 传统模乘算法 | 42.22 ns | 23.69 M ops/sec | 1.00x (基准) | 基准 |
| 优化Montgomery算法 | 28.23 ns | 35.42 M ops/sec | 1.50x | +50.0% |

**注意**: 实际结果会根据测试环境、系统负载等因素有所变化。

### 性能提升分析

- **性能提升**: 通常50%以上（基于实际测试）
- **优化技术**: Montgomery算法、循环展开、内存对齐、编译器优化
- **安全性**: 完全保持原有安全特性
- **测试方法**: 多次迭代平均法（10万次或100万次）

### 性能测试原理

性能测试采用**多次迭代平均法**：

1. **预热阶段**: 执行1000次迭代，消除冷启动影响
2. **测试阶段**: 执行大量迭代（默认100,000次，可自定义）
3. **计算阶段**: 通过总时间除以迭代次数得到平均值

详细原理请参阅：[测评原理.md](测评原理.md)

## 🔧 故障排除

### 常见问题

#### 1. 编译错误
**问题**: `gcc: command not found`  
**解决方案**: 
- 确保安装了MSYS2和GCC编译器
- 使用MSYS2 MINGW64终端而不是PowerShell
- 参考 [MSYS2编译运行指南.md](MSYS2编译运行指南.md)

#### 2. 外部数据加载失败
**问题**: `错误: 未找到有效的测试向量`  
**解决方案**: 
- 检查JSON文件格式是否正确
- 确保文件路径正确
- 使用 `--list` 选项查看可用的测试向量
- 或使用交互式输入方式（选项7 → 选项2）

#### 3. 中文显示乱码
**问题**: 程序中的中文显示为乱码  
**解决方案**: 
- 使用MSYS2 MINGW64终端
- 确保终端支持UTF-8编码
- 程序会自动设置控制台编码

#### 4. 程序找不到
**问题**: `无法运行性能测试程序`  
**解决方案**: 
- 确保已编译相应的程序
- 检查程序是否在当前目录
- 使用 `make` 编译所有程序

### 获取帮助

如果遇到问题，可以:
1. 检查MSYS2环境是否正确安装
2. 确认GCC编译器是否可用
3. 查看错误信息并参考故障排除指南
4. 参考详细文档：
   - [MSYS2编译运行指南.md](MSYS2编译运行指南.md)
   - [外部测试数据使用说明.md](外部测试数据使用说明.md)
   - [测评原理.md](测评原理.md)

## 🎯 项目亮点

1. **算法创新**: 基于Montgomery算法的优化实现
2. **性能卓越**: 相比传统算法提升50%以上（基于实际测试）
3. **安全可靠**: 完全保持原有安全特性
4. **易于使用**: 提供完整的交互式演示
5. **灵活测试**: 支持外部数据和交互式输入
6. **动态分析**: 性能对比结果实时计算
7. **文档完善**: 详细的使用说明和测试结果

## 📚 相关文档

- 📄 [外部测试数据使用说明.md](外部测试数据使用说明.md) - 外部数据功能完整说明
- 📄 [外部数据调用逻辑说明.md](外部数据调用逻辑说明.md) - 技术实现原理
- 📄 [测评原理.md](测评原理.md) - 性能测评原理和方法
- 📄 [MSYS2编译运行指南.md](MSYS2编译运行指南.md) - 编译运行详细指南

## 🚀 快速命令参考

### 编译命令

**交互式演示程序**:
```bash
gcc -O3 -Wall -Isrc -o demo_windows.exe demo_windows.c src/mont_field_simple_optimized.c src/mont_field.c src/traditional_mul.c src/utils.c -lm
```

**内部数据测试**:
```bash
gcc -O3 -Wall -Isrc -o performance_comparison_test.exe performance_comparison_test.c src/mont_field.c src/optimized_montgomery_algorithm.c src/traditional_mul.c src/utils.c -lm
```

**外部数据测试**:
```bash
gcc -O3 -Wall -Isrc -o performance_test_with_external.exe performance_test_with_external.c src/external_test_data.c src/mont_field.c src/optimized_montgomery_algorithm.c src/traditional_mul.c src/utils.c -lm
```

### 运行命令

**交互式演示**:
```bash
./demo_windows.exe
```

**内部数据测试**:
```bash
./performance_comparison_test.exe
```

**外部数据测试**:
```bash
# 使用内置数据
./performance_test_with_external.exe

# 使用外部JSON文件
./performance_test_with_external.exe --external test_data_external.json

# 列出所有测试向量
./performance_test_with_external.exe --external test_data_external.json --list

# 使用指定向量和自定义迭代次数
./performance_test_with_external.exe --external test_data_external.json --vector 2 --iterations 1000000
```

## 📞 技术支持

如需技术支持，请参考:
- 项目文档: 各功能模块的详细说明文档
- 故障排除: 本README的故障排除部分
- 编译指南: [MSYS2编译运行指南.md](MSYS2编译运行指南.md)

---

**版本**: 2.0  
**最后更新**: 2025年  
**维护者**: CSIDH-256 项目组

**祝您使用愉快！** 🎉
