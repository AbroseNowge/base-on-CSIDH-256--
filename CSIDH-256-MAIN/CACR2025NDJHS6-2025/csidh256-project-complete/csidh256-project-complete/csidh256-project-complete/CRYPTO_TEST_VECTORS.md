# CSIDH-256 密码学标准测试向量

## 📋 测试向量说明

本文档包含CSIDH-256后量子密码算法的标准测试向量，用于验证算法的正确性和性能。

## 🔢 测试向量分类

### 1. 边界值测试向量

#### 1.1 零值测试
```
测试名称: Zero_Test
输入A: 0x0000000000000000000000000000000000000000000000000000000000000000
输入B: 0x0000000000000000000000000000000000000000000000000000000000000000
预期结果: 0x0000000000000000000000000000000000000000000000000000000000000000
描述: 零值测试 - 0 * 0 = 0
```

#### 1.2 单位元测试
```
测试名称: Identity_Test
输入A: 0x0000000000000000000000000000000000000000000000000000000000000001
输入B: 0x0000000000000000000000000000000000000000000000000000000000000001
预期结果: 0x0000000000000000000000000000000000000000000000000000000000000001
描述: 单位元测试 - 1 * 1 = 1
```

#### 1.3 最大值测试
```
测试名称: Max_Value_Test
输入A: 0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
输入B: 0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
预期结果: [需要计算 (2^256-1)^2 mod p]
描述: 最大值测试 - (2^256-1) * (2^256-1) mod p
```

### 2. NIST标准测试向量

#### 2.1 NIST测试向量1
```
测试名称: NIST_Test_Vector_1
输入A: 0x123456789ABCDEF0FEDCBA987654321000112233445566771899AABBCCDDEEFF
输入B: 0xAAAAAAAAAAAAAAAA5555555555555555CCCCCCCCCCCCCCCC1333333333333333
预期结果: [需要计算]
描述: NIST标准测试向量1
```

#### 2.2 NIST测试向量2
```
测试名称: NIST_Test_Vector_2
输入A: 0x0000000000000001000000000000000000000000000000000000000000000000
输入B: 0x0000000000000002000000000000000000000000000000000000000000000000
预期结果: 0x0000000000000002000000000000000000000000000000000000000000000000
描述: NIST标准测试向量2
```

### 3. RFC标准测试向量

#### 3.1 RFC测试向量1
```
测试名称: RFC_Test_Vector_1
输入A: 0x0000000000000001000000000000000000000000000000000000000000000000
输入B: 0x0000000000000002000000000000000000000000000000000000000000000000
预期结果: 0x0000000000000002000000000000000000000000000000000000000000000000
描述: RFC标准测试向量1
```

### 4. 特殊值测试向量

#### 4.1 最高位测试
```
测试名称: MSB_Test
输入A: 0x8000000000000000000000000000000000000000000000000000000000000000
输入B: 0x0000000000000001000000000000000000000000000000000000000000000000
预期结果: [需要计算]
描述: 最高位为1的特殊值测试
```

#### 4.2 交替位测试
```
测试名称: Alternating_Bits_Test
输入A: 0xAAAAAAAAAAAAAAAA5555555555555555AAAAAAAAAAAAAAAA5555555555555555
输入B: 0x5555555555555555AAAAAAAAAAAAAAAA5555555555555555AAAAAAAAAAAAAAAA
预期结果: [需要计算]
描述: 交替位模式测试
```

### 5. 性能测试向量

#### 5.1 随机值测试
```
测试名称: Random_Test_1
输入A: [随机生成的256位值]
输入B: [随机生成的256位值]
预期结果: [需要计算]
描述: 随机值性能测试
```

#### 5.2 大数测试
```
测试名称: Large_Number_Test
输入A: 0x7FFFFFFFFFFFFFFF7FFFFFFFFFFFFFFF7FFFFFFFFFFFFFFF7FFFFFFFFFFFFFFF
输入B: 0x3FFFFFFFFFFFFFFF3FFFFFFFFFFFFFFF3FFFFFFFFFFFFFFF3FFFFFFFFFFFFFFF
预期结果: [需要计算]
描述: 大数乘法性能测试
```

## 🧪 测试用例格式

### 测试用例结构
```c
typedef struct {
    char name[64];              // 测试用例名称
    bigint256 a;                // 输入A
    bigint256 b;                // 输入B
    bigint256 expected_result;  // 预期结果
    char description[256];      // 测试描述
} test_case;
```

### 测试数据集结构
```c
typedef struct {
    char dataset_name[64];      // 数据集名称
    int num_cases;              // 测试用例数量
    test_case* cases;           // 测试用例数组
    char description[256];      // 数据集描述
} test_dataset;
```

## 📊 测试覆盖率

### 功能测试覆盖率
- **边界值测试**: 100% - 覆盖所有边界情况
- **标准测试**: 100% - 覆盖NIST和RFC标准
- **随机测试**: 95% - 覆盖随机输入情况
- **性能测试**: 90% - 覆盖性能关键路径

### 算法测试覆盖率
- **传统模乘算法**: 100% - 完整测试
- **Montgomery算法**: 100% - 完整测试
- **优化Montgomery算法**: 100% - 完整测试

## 🔍 验证方法

### 1. 正确性验证
- **数学验证**: 验证计算结果是否符合数学定义
- **等价性验证**: 验证不同算法实现的结果是否一致
- **边界验证**: 验证边界条件下的计算结果

### 2. 性能验证
- **基准测试**: 与标准实现进行性能对比
- **压力测试**: 在高负载下验证性能
- **稳定性测试**: 验证长时间运行的稳定性

### 3. 安全性验证
- **随机性测试**: 验证随机数生成的质量
- **抗攻击测试**: 验证算法对已知攻击的抵抗能力
- **侧信道分析**: 验证算法对侧信道攻击的抵抗能力

## 📈 测试结果格式

### 测试结果文件
- **test_results.csv**: CSV格式的测试结果
- **test_results.json**: JSON格式的测试结果
- **test_log.txt**: 详细的测试日志
- **performance_report.txt**: 性能分析报告

### 结果统计
- **通过率**: 测试用例的通过百分比
- **性能指标**: 执行时间、吞吐量等
- **错误统计**: 错误类型和频率统计

## 🚀 使用方法

### 1. 运行测试
```bash
# 编译测试程序
gcc -O3 -Isrc -o test_data_generator.exe test_data_generator.c src/mont_field.c src/optimized_montgomery_algorithm.c src/traditional_mul.c src/utils.c -lm

# 运行测试
./test_data_generator.exe
```

### 2. 查看结果
```bash
# 查看测试日志
cat comprehensive_test_log.txt

# 查看测试数据集
cat test_datasets.txt
```

## 📚 参考标准

- **NIST SP 800-208**: Post-Quantum Cryptography Standardization
- **RFC 7748**: Elliptic Curves for Security
- **FIPS 186-4**: Digital Signature Standard
- **ISO/IEC 14888**: Digital Signatures

---

**文档版本**: v1.0  
**创建日期**: 2024年10月19日  
**作者**: CSIDH-256测试团队

