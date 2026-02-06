#include "../src/mont_field.h"
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

void test_montgomery_base(void) {
    printf("=== Montgomery Base Version Test ===\n\n");

    mont_field mf;
    mont_field_init(&mf);

    bigint256 a_mont, b_mont, result;

    to_mont(&a_mont, &TEST_A, &mf);
    to_mont(&b_mont, &TEST_B, &mf);

    double start = get_time_ms();
    for (int i = 0; i < 100000; i++) {
        mont_mul(&result, &a_mont, &b_mont, &mf);
    }
    double end = get_time_ms();

    double total_time = end - start;
    double ns_per_op = (total_time * 1e6) / 100000.0;

    printf("Base Version - 100000 multiplications: %.2f ms\n", total_time);
    printf("Time per operation: %.2f ns\n", ns_per_op);
    printf("Throughput: %.2f M ops/sec\n\n", 1000.0 / ns_per_op);
}

void test_montgomery_optimized(void) {
    printf("=== Montgomery Optimized Version Test ===\n\n");

    mont_field mf;
    mont_field_init_optimized(&mf);

    bigint256 a_mont, b_mont, result;

    to_mont_optimized(&a_mont, &TEST_A, &mf);
    to_mont_optimized(&b_mont, &TEST_B, &mf);

    double start = get_time_ms();
    for (int i = 0; i < 100000; i++) {
        mont_mul_optimized(&result, &a_mont, &b_mont, &mf);
    }
    double end = get_time_ms();

    double total_time = end - start;
    double ns_per_op = (total_time * 1e6) / 100000.0;

    printf("Optimized Version - 100000 multiplications: %.2f ms\n", total_time);
    printf("Time per operation: %.2f ns\n", ns_per_op);
    printf("Throughput: %.2f M ops/sec\n\n", 1000.0 / ns_per_op);
}

int main() {
    printf("CSIDH-256 Montgomery Multiplication Performance Comparison Test\n");
    printf("===============================================================\n\n");

    test_montgomery_base();
    test_montgomery_optimized();

    printf("Correctness Verification: %s\n",
           verify_montgomery_correctness() ? "PASS" : "FAIL");

    return 0;
}
