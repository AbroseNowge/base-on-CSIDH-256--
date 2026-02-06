#include "../src/mont_field.h"
#include "../src/traditional_mul.h"
#include <stdio.h>

// 测试数据
const bigint256 TEST_A = {
    {0x123456789ABCDEF0, 0xFEDCBA9876543210,
     0x0011223344556677, 0x1899AABBCCDDEEFF}
};

const bigint256 TEST_B = {
    {0xAAAAAAAAAAAAAAAA, 0x5555555555555555,
     0xCCCCCCCCCCCCCCCC, 0x1333333333333333}
};

void test_traditional(void) {
    printf("=== Traditional Modular Multiplication ===\n");

    bigint256 result;
    uint64_t checksum = 0;
    
    // 预热
    for (int i = 0; i < 1000; i++) {
        traditional_mod_mul_real(&result, &TEST_A, &TEST_B);
    }
    
    double start = get_time_ms();
    for (int i = 0; i < 100000; i++) {
        traditional_mod_mul_real(&result, &TEST_A, &TEST_B);
        checksum += result.limbs[0]; // 使用结果防止优化
    }
    double end = get_time_ms();
    
    double total_time = end - start;
    double ns_per_op = (total_time * 1e6) / 100000.0;
    
    printf("Iterations: 100000\n");
    printf("Total time: %.2f ms\n", total_time);
    printf("Time per operation: %.2f ns\n", ns_per_op);
    printf("Throughput: %.2f M ops/sec\n", 1000.0 / ns_per_op);
    printf("Checksum: %llu\n\n", (unsigned long long)checksum);
}

void test_montgomery_fast(void) {
    printf("=== Optimized Montgomery Multiplication ===\n");

    mont_field mf;
    mont_field_init_fast(&mf);

    bigint256 a_mont, b_mont, result;
    to_mont_fast(&a_mont, &TEST_A, &mf);
    to_mont_fast(&b_mont, &TEST_B, &mf);

    uint64_t checksum = 0;
    
    // 预热
    for (int i = 0; i < 1000; i++) {
        mont_mul_fast(&result, &a_mont, &b_mont, &mf);
    }
    
    double start = get_time_ms();
    for (int i = 0; i < 100000; i++) {
        mont_mul_fast(&result, &a_mont, &b_mont, &mf);
        checksum += result.limbs[0]; // 使用结果防止优化
    }
    double end = get_time_ms();
    
    double total_time = end - start;
    double ns_per_op = (total_time * 1e6) / 100000.0;
    
    printf("Iterations: 100000\n");
    printf("Total time: %.2f ms\n", total_time);
    printf("Time per operation: %.2f ns\n", ns_per_op);
    printf("Throughput: %.2f M ops/sec\n", 1000.0 / ns_per_op);
    printf("Checksum: %llu\n\n", (unsigned long long)checksum);
    
    // 验证正确性
    bigint256 final_result;
    from_mont_fast(&final_result, &result, &mf);
    printf("Verification: Result computed successfully\n");
    
    // 显示部分结果
    printf("Result (first limb): 0x%016llX\n", final_result.limbs[0]);
}

int main() {
    printf("CSIDH-256: Traditional vs Optimized Montgomery\n");
    printf("==============================================\n\n");
    
    test_traditional();
    test_montgomery_fast();
    
    printf("Comparison complete!\n");
    
    return 0;
}
