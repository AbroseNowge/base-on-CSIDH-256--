# CSIDH-256 密码学测试工具集

## 概述

本工具集专为后量子密码CSIDH-256优化方案设计，提供完整的密码学测试数据生成、性能测试和验证功能。工具集包含多种测试向量、性能基准测试和安全验证功能，确保后量子密码算法的正确性和性能。

## 工具组成

### 1. 密码学测试向量生成器 (`crypto_test_vectors_generator.c`)

**功能**: 生成各种标准化的密码学测试向量

**支持的测试向量类型**:
- **NIST SP 800-186标准测试向量**: 符合NIST后量子密码标准的测试向量
- **RFC标准测试向量**: RFC 7748 X25519和RFC 8032 Ed25519测试向量
- **安全测试向量**: 侧信道攻击、时序攻击、故障注入攻击测试向量
- **性能测试向量**: 高负载和随机数据性能测试向量

**输出格式**:
- JSON格式 (`complete_crypto_test_vectors.json`)
- CSV格式 (`crypto_test_vectors.csv`)
- 详细报告 (`crypto_test_vectors_report.txt`)

### 2. 增强版密码学测试收集器 (`enhanced_crypto_test_collector.c`)

**功能**: 运行增强版性能测试，包含详细的性能指标

**测试指标**:
- 执行时间 (纳秒级精度)
- 吞吐量 (M ops/sec)
- 能效比
- 内存使用量
- 缓存命中率
- 安全级别验证

**输出格式**:
- JSON格式 (`enhanced_crypto_test_results.json`)
- CSV格式 (`crypto_validation_results.csv`)
- 性能对比报告 (`enhanced_performance_report.txt`)

### 3. 密码学性能测试数据生成器 (`crypto_performance_test_data_generator.c`)

**功能**: 专门生成用于性能对比的密码学测试数据

**测试类别**:
- NIST标准测试
- RFC标准测试
- 安全测试
- 性能测试

**输出格式**:
- JSON格式 (`crypto_test_vectors.json`)
- CSV格式 (`crypto_performance_benchmark.csv`)
- 性能报告 (`crypto_performance_report.txt`)

### 4. 密码学测试数据验证器 (`crypto_test_data_validator.c`)

**功能**: 验证生成的测试数据的正确性和一致性

**验证内容**:
- 传统算法验证
- Montgomery算法验证
- 校验和验证
- 执行时间验证
- 安全级别验证

**输出格式**:
- 验证日志 (`crypto_validation_log.txt`)
- JSON格式验证结果

## 快速开始

### 环境要求

- Windows 10/11
- MSYS2/MINGW64 或 GCC编译器
- 已编译的CSIDH-256源码

### 编译和运行

1. **使用批处理脚本** (推荐):
   ```bash
   run_crypto_test_tools.bat
   ```

2. **手动编译**:
   ```bash
   # 编译密码学测试向量生成器
   gcc -O3 -Wall -Wextra -std=c99 -I./src crypto_test_vectors_generator.c ./src/mont_field.c ./src/mont_field_optimized.c ./src/traditional_mul.c -o crypto_test_vectors_generator.exe

   # 编译增强版测试收集器
   gcc -O3 -Wall -Wextra -std=c99 -I./src enhanced_crypto_test_collector.c ./src/mont_field.c ./src/mont_field_optimized.c ./src/traditional_mul.c -o enhanced_crypto_test_collector.exe

   # 编译性能测试数据生成器
   gcc -O3 -Wall -Wextra -std=c99 -I./src crypto_performance_test_data_generator.c ./src/mont_field.c ./src/mont_field_optimized.c ./src/traditional_mul.c -o crypto_performance_test_data_generator.exe

   # 编译测试数据验证器
   gcc -O3 -Wall -Wextra -std=c99 -I./src crypto_test_data_validator.c ./src/mont_field.c ./src/mont_field_optimized.c ./src/traditional_mul.c -o crypto_test_data_validator.exe
   ```

### 运行测试

1. **生成完整测试数据**:
   ```bash
   crypto_test_vectors_generator.exe
   # 选择选项1: 生成完整测试向量集合
   ```

2. **运行性能测试**:
   ```bash
   enhanced_crypto_test_collector.exe
   ```

3. **验证测试数据**:
   ```bash
   crypto_test_data_validator.exe
   # 选择选项1: 运行标准测试向量验证
   ```

## 测试向量说明

### NIST SP 800-186 测试向量

这些测试向量符合NIST后量子密码标准，用于验证算法的标准符合性：

```c
// 示例测试向量
bigint256 nist_a = {
    .limbs = {0x123456789ABCDEF0, 0xFEDCBA9876543210, 0x0011223344556677, 0x1899AABBCCDDEEFF}
};

bigint256 nist_b = {
    .limbs = {0xAAAAAAAAAAAAAAAA, 0x5555555555555555, 0xCCCCCCCCCCCCCCCC, 0x1333333333333333}
};
```

### 安全测试向量

专门设计用于检测各种安全攻击的测试向量：

- **侧信道攻击测试**: 使用最高位为1的数据检测功率分析攻击
- **时序攻击测试**: 使用零值和最大值检测时序分析攻击
- **故障注入攻击测试**: 使用特殊值检测故障注入攻击

### 性能测试向量

用于性能基准测试的向量：

- **高负载测试**: 大数据量处理性能测试
- **随机数据测试**: 随机输入数据处理性能测试

## 性能指标说明

### 执行时间 (Execution Time)
- 单位: 纳秒 (ns)
- 精度: 微秒级
- 用途: 衡量算法执行效率

### 吞吐量 (Throughput)
- 单位: M ops/sec (百万次操作/秒)
- 计算: 1000 / 平均执行时间(ns)
- 用途: 衡量算法处理能力

### 能效比 (Energy Efficiency)
- 计算: 吞吐量 / 执行时间
- 用途: 衡量算法的能耗效率

### 内存使用量 (Memory Usage)
- 单位: MB
- 用途: 衡量算法内存消耗

### 缓存命中率 (Cache Hit Ratio)
- 单位: 百分比 (%)
- 用途: 衡量算法缓存性能

## 输出文件说明

### JSON格式文件
- 结构化数据，便于程序解析
- 包含完整的元数据信息
- 支持嵌套结构

### CSV格式文件
- 表格数据，便于Excel分析
- 包含性能指标和验证结果
- 易于数据可视化

### 文本报告文件
- 人类可读的详细报告
- 包含测试统计和分析
- 适合文档和展示

## 安全考虑

### 威胁模型

本工具集考虑了以下威胁模型：

1. **侧信道攻击**: 功率分析攻击、电磁攻击
2. **时序攻击**: 缓存攻击、分支预测攻击
3. **故障注入攻击**: 电磁故障注入、激光故障注入
4. **密码学分析攻击**: 差分攻击、线性攻击

### 安全级别

- **级别1**: 基础安全
- **级别2**: 标准安全
- **级别3**: 高安全
- **级别4**: 极高安全
- **级别5**: 最高安全 (后量子安全)

## 使用建议

### 测试流程

1. **生成测试向量**: 首先运行测试向量生成器
2. **运行性能测试**: 使用生成的向量进行性能测试
3. **验证结果**: 使用验证器验证测试结果的正确性
4. **分析报告**: 查看生成的详细报告

### 性能优化建议

1. **使用-O3优化**: 编译时使用最高优化级别
2. **预热阶段**: 测试前进行足够的预热
3. **多次测试**: 进行多次测试取平均值
4. **环境一致**: 保持测试环境的一致性

### 安全测试建议

1. **全面测试**: 运行所有类型的测试向量
2. **安全验证**: 特别关注安全测试向量的结果
3. **威胁分析**: 根据威胁模型分析结果
4. **持续监控**: 定期运行安全验证

## 故障排除

### 常见问题

1. **编译错误**: 检查GCC版本和头文件路径
2. **运行时错误**: 检查依赖库是否正确链接
3. **测试失败**: 检查输入数据格式和算法实现
4. **性能异常**: 检查系统负载和优化设置

### 调试建议

1. **启用调试信息**: 编译时使用-g选项
2. **检查日志**: 查看生成的日志文件
3. **单步测试**: 单独运行每个测试工具
4. **环境检查**: 验证测试环境配置

## 扩展开发

### 添加新的测试向量

1. 在相应的生成函数中添加新的测试向量
2. 更新测试向量结构定义
3. 修改输出格式以包含新向量
4. 更新文档和报告

### 添加新的性能指标

1. 扩展测试结果结构
2. 修改测试函数以计算新指标
3. 更新输出格式
4. 修改报告生成逻辑

### 集成新的算法

1. 添加新的算法函数声明
2. 实现相应的测试函数
3. 更新验证逻辑
4. 修改性能对比分析

## 许可证

本工具集遵循项目的开源许可证。

## 贡献

欢迎提交问题和改进建议！

## 联系方式

如有问题，请通过项目仓库提交issue。


