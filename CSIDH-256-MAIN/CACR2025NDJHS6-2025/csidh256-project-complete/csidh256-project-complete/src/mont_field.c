#include "mont_field.h"
#include <string.h>

// ==================== 大数基础运算 ====================

void bigint_add(bigint256 *result, const bigint256 *a, const bigint256 *b) {
    uint64_t carry = 0;
    for (int i = 0; i < LIMBS; i++) {
        __uint128_t sum = (__uint128_t)a->limbs[i] + b->limbs[i] + carry;
        result->limbs[i] = (uint64_t)sum;
        carry = (uint64_t)(sum >> 64);
    }
}

void bigint_sub(bigint256 *result, const bigint256 *a, const bigint256 *b, const bigint256 *p) {
    uint64_t borrow = 0;
    for (int i = 0; i < LIMBS; i++) {
        __uint128_t diff = (__uint128_t)a->limbs[i] - b->limbs[i] - borrow;
        result->limbs[i] = (uint64_t)diff;
        borrow = (diff >> 64) ? 1 : 0;
    }
    
    if (borrow) {
        bigint_add(result, result, p);
    }
}

int bigint_compare(const bigint256 *a, const bigint256 *b) {
    for (int i = LIMBS - 1; i >= 0; i--) {
        if (a->limbs[i] > b->limbs[i]) return 1;
        if (a->limbs[i] < b->limbs[i]) return -1;
    }
    return 0;
}

// ==================== 大数乘法（优化版本）====================

static void bigint_mul_512_optimized(uint64_t *result, const bigint256 *a, const bigint256 *b) {
    // 使用__builtin_memset（编译器优化）
    __builtin_memset(result, 0, 8 * sizeof(uint64_t));
    
    // 优化的乘法：减少内存访问
    for (int i = 0; i < LIMBS; i++) {
        uint64_t carry = 0;
        const uint64_t a_limb = a->limbs[i];  // 缓存，减少内存访问
        for (int j = 0; j < LIMBS; j++) {
            __uint128_t prod = (__uint128_t)a_limb * b->limbs[j] + result[i + j] + carry;
            result[i + j] = (uint64_t)prod;
            carry = (uint64_t)(prod >> 64);
        }
        result[i + LIMBS] = carry;
    }
}

// 保留原函数用于其他地方
static void bigint_mul_512(uint64_t *result, const bigint256 *a, const bigint256 *b) {
    bigint_mul_512_optimized(result, a, b);
}

// ==================== Montgomery 约简（优化版本）====================

static void mont_redc(bigint256 *result, const uint64_t *T, const mont_field *mf) {
    // 优化：直接使用内联计算，减少内存拷贝
    uint64_t temp[8];
    // 使用memcpy优化（编译器会优化）
    __builtin_memcpy(temp, T, 8 * sizeof(uint64_t));
    
    // 优化的Montgomery约简：内联展开，减少循环开销
    for (int i = 0; i < LIMBS; i++) {
        uint64_t m_i = temp[i] * mf->p_inv.limbs[0];
        
        uint64_t carry = 0;
        // 展开内层循环（部分展开以提高性能）
        for (int j = 0; j < LIMBS; j++) {
            __uint128_t prod = (__uint128_t)m_i * mf->p.limbs[j] + temp[i + j] + carry;
            temp[i + j] = (uint64_t)prod;
            carry = (uint64_t)(prod >> 64);
        }
        
        // 优化的进位传播：使用更高效的方式
        int idx = i + LIMBS;
        if (carry > 0 && idx < 8) {
            __uint128_t sum = (__uint128_t)temp[idx] + carry;
            temp[idx] = (uint64_t)sum;
            carry = (uint64_t)(sum >> 64);
            idx++;
            if (carry > 0 && idx < 8) {
                sum = (__uint128_t)temp[idx] + carry;
                temp[idx] = (uint64_t)sum;
            }
        }
    }
    
    // 直接复制结果（避免额外的memcpy）
    __builtin_memcpy(result->limbs, &temp[LIMBS], LIMBS * sizeof(uint64_t));
    
    // 优化的最终约简：只比较一次
    if (bigint_compare(result, &mf->p) >= 0) {
        bigint_sub(result, result, &mf->p, &mf->p);
    }
}

// ==================== Montgomery 乘法（优化版本）====================

void mont_mul(bigint256 *result, const bigint256 *a, const bigint256 *b, const mont_field *mf) {
    uint64_t T[8];
    bigint_mul_512_optimized(T, a, b);
    mont_redc(result, T, mf);
}

// ==================== 辅助函数 ====================

static uint64_t inv_mod_2_64(uint64_t a) {
    uint64_t x = a;
    for (int i = 0; i < 6; i++) {
        x = x * (2 - a * x);
    }
    return x;
}

// ==================== Montgomery 域初始化 ====================

void mont_field_init(mont_field *mf) {
    memcpy(mf->p.limbs, CSIDH256_P, LIMBS * sizeof(uint64_t));
    
    mf->p_inv.limbs[0] = -inv_mod_2_64(mf->p.limbs[0]);
    for (int i = 1; i < LIMBS; i++) {
        mf->p_inv.limbs[i] = 0;
    }
    
    bigint256 R;
    memset(&R, 0, sizeof(R));
    R.limbs[0] = 1;
    
    for (int i = 0; i < 256; i++) {
        bigint_add(&R, &R, &R);
        if (bigint_compare(&R, &mf->p) >= 0) {
            bigint_sub(&R, &R, &mf->p, &mf->p);
        }
    }
    
    mont_mul(&mf->r_squared, &R, &R, mf);
}

// ==================== 转换函数 ====================

void to_mont(bigint256 *result, const bigint256 *a, const mont_field *mf) {
    mont_mul(result, a, &mf->r_squared, mf);
}

void from_mont(bigint256 *result, const bigint256 *a, const mont_field *mf) {
    uint64_t T[8];
    memset(T, 0, 8 * sizeof(uint64_t));
    memcpy(T, a->limbs, LIMBS * sizeof(uint64_t));
    mont_redc(result, T, mf);
}
