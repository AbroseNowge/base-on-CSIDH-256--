# CSIDH-256 测试数据文件

## 📊 测试数据集

### 测试数据1：标准测试用例
```c
// 测试数据A - 标准256位大整数
const bigint256 TEST_A = {
    {0x123456789ABCDEF0, 0xFEDCBA9876543210,
     0x0011223344556677, 0x1899AABBCCDDEEFF}
};

// 测试数据B - 标准256位大整数
const bigint256 TEST_B = {
    {0xAAAAAAAAAAAAAAAA, 0x5555555555555555,
     0xCCCCCCCCCCCCCCCC, 0x1333333333333333}
};
```

### 测试数据2：边界测试用例
```c
// 最大值测试
const bigint256 MAX_VALUE = {
    {0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF,
     0xFFFFFFFFFFFFFFFF, 0x1FFFFFFFFFFFFFFF}
};

// 最小值测试
const bigint256 MIN_VALUE = {
    {0x0000000000000001, 0x0000000000000000,
     0x0000000000000000, 0x0000000000000000}
};

// 零值测试
const bigint256 ZERO_VALUE = {
    {0x0000000000000000, 0x0000000000000000,
     0x0000000000000000, 0x0000000000000000}
};
```

### 测试数据3：随机测试用例
```c
// 随机测试数据1
const bigint256 RANDOM_1 = {
    {0x7A3B8C9D2E4F6A1B, 0x5C7D8E9F0A1B2C3D,
     0x4E5F6A7B8C9D0E1F, 0x2A3B4C5D6E7F8A9B}
};

// 随机测试数据2
const bigint256 RANDOM_2 = {
    {0x9F8E7D6C5B4A3928, 0x1F0E9D8C7B6A5948,
     0x3F2E1D0C9B8A7968, 0x5F4E3D2C1B0A9F8E}
};
```

## 🧪 测试用例说明

### 性能测试参数
- **迭代次数**: 100,000次
- **预热次数**: 1,000次
- **测试环境**: Windows 10/11
- **编译器**: GCC with -O3 optimization
- **CPU**: x86_64 architecture

### 正确性测试参数
- **测试方法**: 校验和验证
- **验证方式**: 输入输出一致性检查
- **数学验证**: Montgomery约简等价性验证

## 📈 预期测试结果

### 性能基准
- **我们的优化算法**: 预期 25-30 ns/op
- **传统算法**: 预期 40-50 ns/op
- **性能提升**: 预期 30-40%

### 正确性验证
- **校验和**: 非零值表示计算正确
- **结果一致性**: 相同输入产生相同输出
- **数学等价性**: Montgomery算法与Barrett算法结果一致

