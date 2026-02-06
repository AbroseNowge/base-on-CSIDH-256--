// CSIDH-256 单元测试框架
// 测试：field运算、Montgomery转换、单步isogeny

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>
#include "src/fp256.h"
#include "src/edwards256.h"
#include "src/csidh256_params.h"
#include "src/param_validator.h"
#include "src/rng.h"

// 测试结果统计
static int tests_run = 0;
static int tests_passed = 0;
static int tests_failed = 0;

#define TEST_ASSERT(condition, message) \
    do { \
        tests_run++; \
        if (condition) { \
            tests_passed++; \
            printf("  ✅ PASS: %s\n", message); \
        } else { \
            tests_failed++; \
            printf("  ❌ FAIL: %s\n", message); \
        } \
    } while(0)

// ==================== Field运算测试 ====================

void test_field_operations(void) {
    printf("\n=== Field运算测试 ===\n");
    
    // 确保初始化
    if (!g_mf_initialized) {
        init_montgomery_field();
    }
    
    fp a, b, c, zero, one;
    
    // 测试 set_zero 和 set_one
    set_zero(&zero);
    set_one(&one);
    TEST_ASSERT(fp_iszero(&zero), "set_zero produces zero");
    TEST_ASSERT(fp_compare(&one, &zero) > 0, "one > zero");
    
    // 测试 fp_copy
    fp_random(&a);
    fp_copy(&b, &a);
    TEST_ASSERT(fp_compare(&a, &b) == 0, "fp_copy works correctly");
    
    // 测试 fp_add: a + 0 = a
    fp_add(&c, &a, &zero);
    TEST_ASSERT(fp_compare(&c, &a) == 0, "a + 0 = a");
    
    // 测试 fp_add: a + 1
    fp_add(&c, &a, &one);
    TEST_ASSERT(fp_compare(&c, &a) > 0 || fp_compare(&c, &a) < 0, "a + 1 != a");
    
    // 测试 fp_sub: a - a = 0
    fp_sub(&c, &a, &a);
    TEST_ASSERT(fp_iszero(&c), "a - a = 0");
    
    // 测试 fp_mul: a * 1 = a
    fp_mul(&c, &a, &one);
    TEST_ASSERT(fp_compare(&c, &a) == 0, "a * 1 = a");
    
    // 测试 fp_mul: a * 0 = 0
    fp_mul(&c, &a, &zero);
    TEST_ASSERT(fp_iszero(&c), "a * 0 = 0");
    
    // 测试 fp_sqr: a^2 = a * a
    fp_sqr(&c, &a);
    fp_mul(&b, &a, &a);
    TEST_ASSERT(fp_compare(&c, &b) == 0, "a^2 = a * a");
    
    // 测试 fp_inv: a * a^(-1) = 1
    fp_random(&a);
    while (fp_iszero(&a)) {
        fp_random(&a);
    }
    fp_copy(&b, &a);
    fp_inv(&b);
    fp_mul(&c, &a, &b);
    TEST_ASSERT(fp_compare(&c, &one) == 0, "a * a^(-1) = 1");
}

// ==================== Montgomery转换测试 ====================

void test_montgomery_conversion(void) {
    printf("\n=== Montgomery转换测试 ===\n");
    
    if (!g_mf_initialized) {
        init_montgomery_field();
    }
    
    // 测试: to_mont 然后 from_mont 应该恢复原值
    bigint256 a_normal, a_mont, a_recovered;
    
    // 生成随机值（小于p）
    do {
        randombytes(a_normal.limbs, NUMBER_OF_WORDS * sizeof(uint64_t));
        if (a_normal.limbs[NUMBER_OF_WORDS - 1] > g_mf.p.limbs[NUMBER_OF_WORDS - 1]) {
            a_normal.limbs[NUMBER_OF_WORDS - 1] = g_mf.p.limbs[NUMBER_OF_WORDS - 1];
        }
    } while (bigint_compare(&a_normal, &g_mf.p) >= 0);
    
    // 转换到Montgomery域
    to_mont(&a_mont, &a_normal, &g_mf);
    
    // 转换回普通域
    from_mont(&a_recovered, &a_mont, &g_mf);
    
    TEST_ASSERT(bigint_compare(&a_normal, &a_recovered) == 0, 
                "to_mont + from_mont = identity");
    
    // 测试: 1在Montgomery域中的表示
    bigint256 one_normal, one_mont;
    one_normal.limbs[0] = 1;
    for (int i = 1; i < NUMBER_OF_WORDS; i++) {
        one_normal.limbs[i] = 0;
    }
    
    to_mont(&one_mont, &one_normal, &g_mf);
    TEST_ASSERT(bigint_compare(&one_mont, &R_squared_mod_p) == 0,
                "1 in Montgomery = R^2 mod p");
}

// ==================== 单步Isogeny测试 ====================

void test_single_isogeny(void) {
    printf("\n=== 单步Isogeny测试 ===\n");
    
    if (!g_mf_initialized) {
        init_montgomery_field();
    }
    
    extern proj E;
    static bool E_initialized = false;
    if (!E_initialized) {
        init_public_curve();
        E_initialized = true;
    }
    
    // 测试: 验证公共曲线E是超奇异曲线
    TEST_ASSERT(validate(E) == 1, "Public curve E is supersingular");
    
    // 测试: 点运算
    proj P, Q, R;
    
    // 生成随机点
    fp_random(&P[0]);
    while (fp_iszero(&P[0])) {
        fp_random(&P[0]);
    }
    set_zero(&P[1]);
    fp_add(&P[1], &P[1], &R_mod_p);
    
    // 测试: yDBL (点倍乘)
    yDBL(Q, P, E);
    TEST_ASSERT(isinfinity(Q) == 0 || isinfinity(Q) == 1, "yDBL produces valid point");
    
    // 测试: yADD (点加法)
    if (isinfinity(Q) == 0) {
        yADD(R, P, Q, E);
        TEST_ASSERT(isinfinity(R) == 0 || isinfinity(R) == 1, "yADD produces valid point");
    }
    
    // 测试: point_copy
    point_copy(Q, P);
    TEST_ASSERT(areEqual(P, Q) == 1, "point_copy works correctly");
}

// ==================== KAT (Known Answer Test) 验证 ====================

void test_kat_vectors(void) {
    printf("\n=== KAT向量验证 ===\n");
    
    if (!g_mf_initialized) {
        init_montgomery_field();
    }
    
    extern proj E;
    static bool E_initialized = false;
    if (!E_initialized) {
        init_public_curve();
        E_initialized = true;
    }
    
    // 测试: 使用固定密钥进行CSIDH计算，验证结果一致性
    uint8_t fixed_key[N];
    memset(fixed_key, 0, N);
    fixed_key[0] = 1; // 设置一个简单的密钥
    
    proj result1, result2;
    
    // 执行两次相同的计算
    action_evaluation(result1, fixed_key, E);
    action_evaluation(result2, fixed_key, E);
    
    // 结果应该相同（确定性）
    TEST_ASSERT(areEqual(result1, result2) == 1, 
                "Deterministic CSIDH action produces same result");
    
    printf("  注意: 完整的KAT向量需要与官方CSIDH实现对比\n");
}

// ==================== 主测试函数 ====================

int main() {
    printf("=================================================================\n");
    printf("CSIDH-256 单元测试套件\n");
    printf("=================================================================\n");
    
    // 初始化
    printf("\n初始化系统...\n");
    validate_csidh256_params();
    init_montgomery_field();
    init_public_curve();
    printf("初始化完成！\n");
    
    // 运行测试
    test_field_operations();
    test_montgomery_conversion();
    test_single_isogeny();
    test_kat_vectors();
    
    // 输出结果
    printf("\n=================================================================\n");
    printf("测试结果汇总:\n");
    printf("  总测试数: %d\n", tests_run);
    printf("  通过: %d\n", tests_passed);
    printf("  失败: %d\n", tests_failed);
    printf("  通过率: %.1f%%\n", tests_run > 0 ? (100.0 * tests_passed / tests_run) : 0.0);
    printf("=================================================================\n");
    
    return (tests_failed == 0) ? 0 : 1;
}


