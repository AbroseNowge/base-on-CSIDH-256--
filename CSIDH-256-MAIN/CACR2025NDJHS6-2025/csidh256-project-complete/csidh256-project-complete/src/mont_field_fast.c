#include "mont_field.h"
#include <string.h>

// 快速版 Montgomery 乘法
void mont_mul_fast(bigint256 *result, const bigint256 *a, const bigint256 *b, const mont_field *mf) {
    uint64_t T[8] = {0};
    
    // 256-bit 乘法
    for (int i = 0; i < LIMBS; i++) {
        uint64_t carry = 0;
        for (int j = 0; j < LIMBS; j++) {
            __uint128_t product = (__uint128_t)a->limbs[i] * b->limbs[j] + T[i + j] + carry;
            T[i + j] = (uint64_t)product;
            carry = (uint64_t)(product >> 64);
        }
        T[i + LIMBS] = carry;
    }

    // Montgomery 约简
    for (int i = 0; i < LIMBS; i++) {
        uint64_t m = T[i] * mf->p_inv.limbs[0];
        uint64_t carry = 0;
        
        for (int j = 0; j < LIMBS; j++) {
            __uint128_t product = (__uint128_t)m * mf->p.limbs[j] + T[i + j] + carry;
            T[i + j] = (uint64_t)product;
            carry = (uint64_t)(product >> 64);
        }
        
        // 传播进位
        for (int k = i + LIMBS; k < 8 && carry; k++) {
            __uint128_t sum = (__uint128_t)T[k] + carry;
            T[k] = (uint64_t)sum;
            carry = (uint64_t)(sum >> 64);
        }
    }

    // 复制结果
    memcpy(result->limbs, &T[LIMBS], LIMBS * sizeof(uint64_t));
    
    // 最终约简
    if (bigint_compare(result, &mf->p) >= 0) {
        bigint_sub(result, result, &mf->p, &mf->p);
    }
}

// 快速初始化
void mont_field_init_fast(mont_field *mf) {
    memcpy(mf->p.limbs, CSIDH256_P, LIMBS * sizeof(uint64_t));

    // 计算 p_inv
    uint64_t a = mf->p.limbs[0];
    uint64_t x = a;
    for (int i = 0; i < 6; i++) {
        x = x * (2 - a * x);
    }
    mf->p_inv.limbs[0] = -x;
    for (int i = 1; i < LIMBS; i++) {
        mf->p_inv.limbs[i] = 0;
    }

    // 计算 R^2 mod p
    bigint256 R = {0};
    R.limbs[0] = 1;
    
    for (int i = 0; i < 256; i++) {
        bigint_add(&R, &R, &R);
        if (bigint_compare(&R, &mf->p) >= 0) {
            bigint_sub(&R, &R, &mf->p, &mf->p);
        }
    }
    
    mont_mul_fast(&mf->r_squared, &R, &R, mf);
}

void to_mont_fast(bigint256 *result, const bigint256 *a, const mont_field *mf) {
    mont_mul_fast(result, a, &mf->r_squared, mf);
}

void from_mont_fast(bigint256 *result, const bigint256 *a, const mont_field *mf) {
    uint64_t T[8] = {0};
    memcpy(T, a->limbs, LIMBS * sizeof(uint64_t));
    
    // 直接应用 Montgomery 约简
    for (int i = 0; i < LIMBS; i++) {
        uint64_t m = T[i] * mf->p_inv.limbs[0];
        uint64_t carry = 0;
        
        for (int j = 0; j < LIMBS; j++) {
            __uint128_t product = (__uint128_t)m * mf->p.limbs[j] + T[i + j] + carry;
            T[i + j] = (uint64_t)product;
            carry = (uint64_t)(product >> 64);
        }
        
        for (int k = i + LIMBS; k < 8 && carry; k++) {
            __uint128_t sum = (__uint128_t)T[k] + carry;
            T[k] = (uint64_t)sum;
            carry = (uint64_t)(sum >> 64);
        }
    }
    
    memcpy(result->limbs, &T[LIMBS], LIMBS * sizeof(uint64_t));
    
    if (bigint_compare(result, &mf->p) >= 0) {
        bigint_sub(result, result, &mf->p, &mf->p);
    }
}
