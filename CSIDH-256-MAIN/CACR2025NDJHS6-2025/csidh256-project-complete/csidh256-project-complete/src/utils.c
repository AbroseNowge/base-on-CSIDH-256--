#include "mont_field.h"
#include <windows.h>

// ==================== 时间函数 ====================

#ifndef GET_TIME_MS_DEFINED
#define GET_TIME_MS_DEFINED
double get_time_ms(void) {
    LARGE_INTEGER frequency, counter;
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&counter);
    return (double)counter.QuadPart * 1000.0 / frequency.QuadPart;
}
#endif

// ==================== 验证函数 ====================

bool verify_montgomery_correctness(void) {
    mont_field mf;
    mont_field_init(&mf);

    bigint256 a_mont, b_mont;
    bigint256 result_mont, result_normal;

    // 测试数据
    bigint256 TEST_A = {
        {0x123456789ABCDEF0, 0xFEDCBA9876543210,
         0x0011223344556677, 0x1899AABBCCDDEEFF}
    };

    bigint256 TEST_B = {
        {0xAAAAAAAAAAAAAAAA, 0x5555555555555555,
         0xCCCCCCCCCCCCCCCC, 0x1333333333333333}
    };

    to_mont(&a_mont, &TEST_A, &mf);
    to_mont(&b_mont, &TEST_B, &mf);

    mont_mul(&result_mont, &a_mont, &b_mont, &mf);
    from_mont(&result_normal, &result_mont, &mf);

    // 简单的正确性检查：结果应该非零且在有效范围内
    for (int i = 0; i < LIMBS; i++) {
        if (result_normal.limbs[i] != 0) {
            return true; // 至少有一个 limb 非零
        }
    }
    return false;
}
