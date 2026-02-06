#include "../src/mont_field.h"
#include <stdio.h>
#include <time.h>
#include <windows.h>

// 移除这里的 get_time_ms 函数定义，使用 utils.c 中的版本

#define ITERATIONS 1000000  // 256 可以测试更多次

int main() {
    printf("=== CSIDH-256 Montgomery Multiplication Performance Test ===\n\n");

    // 初始化 Montgomery 域
    mont_field mf;
    printf("Initializing Montgomery field...\n");
    mont_field_init(&mf);
    printf("Initialization complete!\n\n");

    // 测试数据
    bigint256 a = {{0x123456789ABCDEF0, 0xFEDCBA9876543210,
                    0x0011223344556677, 0x1899AABBCCDDEEFF}};

    bigint256 b = {{0xAAAAAAAAAAAAAAAA, 0x5555555555555555,
                    0xCCCCCCCCCCCCCCCC, 0x1333333333333333}};

    bigint256 result;
    bigint256 a_mont, b_mont;

    // 转换到 Montgomery 域
    to_mont(&a_mont, &a, &mf);
    to_mont(&b_mont, &b, &mf);

    printf("Testing Montgomery multiplication...\n");
    printf("Running %d iterations...\n\n", ITERATIONS);

    double start = get_time_ms();

    for (int i = 0; i < ITERATIONS; i++) {
        mont_mul(&result, &a_mont, &b_mont, &mf);
    }

    double end = get_time_ms();
    double total_time = end - start;
    double ns_per_op = (total_time * 1e6) / ITERATIONS; // 纳秒

    printf("=== Results ===\n");
    printf("Iterations: %d\n", ITERATIONS);
    printf("Total time: %.2f ms\n", total_time);
    printf("Time per operation: %.2f ns\n", ns_per_op);
    printf("Throughput: %.2f M ops/sec\n", 1000.0 / ns_per_op);

    // 验证正确性
    bigint256 result_normal;
    from_mont(&result_normal, &result, &mf);

    printf("\n=== Result Verification ===\n");
    printf("First 4 limbs of result:\n");
    for (int i = 0; i < LIMBS; i++) {
        printf("  limb[%d] = 0x%016llX\n", i, result_normal.limbs[i]);
    }

    printf("\n=== Test Complete ===\n");
    printf("\n💡 提示：单次操作时间 %.2f ns 可用于性能对比！\n", ns_per_op);

    return 0;
}
