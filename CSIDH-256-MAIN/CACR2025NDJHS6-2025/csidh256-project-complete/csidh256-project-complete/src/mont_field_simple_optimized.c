#include "mont_field.h"
#include <string.h>

// 简化的Montgomery乘法 - 真正优化版本
void mont_mul_optimized(bigint256 *result, const bigint256 *a, const bigint256 *b, const mont_field *mf) {
    uint64_t T[8] = {0};
    
    // 简化的256-bit乘法
    for (int i = 0; i < LIMBS; i++) {
        uint64_t carry = 0;
        for (int j = 0; j < LIMBS; j++) {
            __uint128_t product = (__uint128_t)a->limbs[i] * b->limbs[j] + T[i + j] + carry;
            T[i + j] = (uint64_t)product;
            carry = (uint64_t)(product >> 64);
        }
        T[i + LIMBS] = carry;
    }

    // 简化的Montgomery约简
    for (int i = 0; i < LIMBS; i++) {
        uint64_t m = T[i] * mf->p_inv.limbs[0];
        uint64_t carry = 0;
        
        for (int j = 0; j < LIMBS; j++) {
            __uint128_t product = (__uint128_t)m * mf->p.limbs[j] + T[i + j] + carry;
            T[i + j] = (uint64_t)product;
            carry = (uint64_t)(product >> 64);
        }
        
        // 传播进位
        int k = i + LIMBS;
        while (carry && k < 8) {
            __uint128_t sum = (__uint128_t)T[k] + carry;
            T[k] = (uint64_t)sum;
            carry = (uint64_t)(sum >> 64);
            k++;
        }
    }

    // 直接赋值结果
    result->limbs[0] = T[4];
    result->limbs[1] = T[5];
    result->limbs[2] = T[6];
    result->limbs[3] = T[7];
    
    // 简化的最终约简
    if (result->limbs[3] > mf->p.limbs[3] || 
        (result->limbs[3] == mf->p.limbs[3] && 
         (result->limbs[2] > mf->p.limbs[2] ||
          (result->limbs[2] == mf->p.limbs[2] &&
           (result->limbs[1] > mf->p.limbs[1] ||
            (result->limbs[1] == mf->p.limbs[1] && result->limbs[0] >= mf->p.limbs[0])))))) {
        
        uint64_t borrow = 0;
        uint64_t diff0 = result->limbs[0] - mf->p.limbs[0];
        uint64_t diff1 = result->limbs[1] - mf->p.limbs[1] - (diff0 > result->limbs[0]);
        uint64_t diff2 = result->limbs[2] - mf->p.limbs[2] - (diff1 > result->limbs[1]);
        uint64_t diff3 = result->limbs[3] - mf->p.limbs[3] - (diff2 > result->limbs[2]);
        
        result->limbs[0] = diff0;
        result->limbs[1] = diff1;
        result->limbs[2] = diff2;
        result->limbs[3] = diff3;
    }
}

// 简化的Montgomery初始化
void mont_field_init_optimized(mont_field *mf) {
    // 直接赋值
    mf->p.limbs[0] = CSIDH256_P[0];
    mf->p.limbs[1] = CSIDH256_P[1];
    mf->p.limbs[2] = CSIDH256_P[2];
    mf->p.limbs[3] = CSIDH256_P[3];

    // 简化的模逆计算
    uint64_t a = mf->p.limbs[0];
    uint64_t x = 1;
    
    // 牛顿迭代
    x = x * (2 - a * x);
    x = x * (2 - a * x);
    x = x * (2 - a * x);
    x = x * (2 - a * x);
    
    mf->p_inv.limbs[0] = -x;
    mf->p_inv.limbs[1] = 0;
    mf->p_inv.limbs[2] = 0;
    mf->p_inv.limbs[3] = 0;

    // 简化的R^2计算
    bigint256 R = {0};
    R.limbs[0] = 1;
    
    // 计算R = 2^256 mod p
    for (int i = 0; i < 256; i++) {
        uint64_t carry = 0;
        for (int j = 0; j < LIMBS; j++) {
            uint64_t old_val = R.limbs[j];
            R.limbs[j] = (R.limbs[j] << 1) | carry;
            carry = old_val >> 63;
        }
        
        // 快速模约简检查
        if (R.limbs[3] > mf->p.limbs[3] || 
            (R.limbs[3] == mf->p.limbs[3] && 
             (R.limbs[2] > mf->p.limbs[2] ||
              (R.limbs[2] == mf->p.limbs[2] &&
               (R.limbs[1] > mf->p.limbs[1] ||
                (R.limbs[1] == mf->p.limbs[1] && R.limbs[0] >= mf->p.limbs[0])))))) {
            
            uint64_t borrow = 0;
            uint64_t diff0 = R.limbs[0] - mf->p.limbs[0];
            uint64_t diff1 = R.limbs[1] - mf->p.limbs[1] - (diff0 > R.limbs[0]);
            uint64_t diff2 = R.limbs[2] - mf->p.limbs[2] - (diff1 > R.limbs[1]);
            uint64_t diff3 = R.limbs[3] - mf->p.limbs[3] - (diff2 > R.limbs[2]);
            
            R.limbs[0] = diff0;
            R.limbs[1] = diff1;
            R.limbs[2] = diff2;
            R.limbs[3] = diff3;
        }
    }
    
    // 计算R^2 mod p
    mont_mul_optimized(&mf->r_squared, &R, &R, mf);
}

// 简化的转换函数
void to_mont_optimized(bigint256 *result, const bigint256 *a, const mont_field *mf) {
    mont_mul_optimized(result, a, &mf->r_squared, mf);
}

void from_mont_optimized(bigint256 *result, const bigint256 *a, const mont_field *mf) {
    uint64_t T[8] = {0};
    
    // 直接赋值
    T[0] = a->limbs[0];
    T[1] = a->limbs[1];
    T[2] = a->limbs[2];
    T[3] = a->limbs[3];
    
    // 简化的Montgomery约简
    for (int i = 0; i < LIMBS; i++) {
        uint64_t m = T[i] * mf->p_inv.limbs[0];
        uint64_t carry = 0;
        
        for (int j = 0; j < LIMBS; j++) {
            __uint128_t product = (__uint128_t)m * mf->p.limbs[j] + T[i + j] + carry;
            T[i + j] = (uint64_t)product;
            carry = (uint64_t)(product >> 64);
        }
        
        // 传播进位
        int k = i + LIMBS;
        while (carry && k < 8) {
            __uint128_t sum = (__uint128_t)T[k] + carry;
            T[k] = (uint64_t)sum;
            carry = (uint64_t)(sum >> 64);
            k++;
        }
    }

    // 直接赋值结果
    result->limbs[0] = T[4];
    result->limbs[1] = T[5];
    result->limbs[2] = T[6];
    result->limbs[3] = T[7];
    
    // 简化的最终约简
    if (result->limbs[3] > mf->p.limbs[3] || 
        (result->limbs[3] == mf->p.limbs[3] && 
         (result->limbs[2] > mf->p.limbs[2] ||
          (result->limbs[2] == mf->p.limbs[2] &&
           (result->limbs[1] > mf->p.limbs[1] ||
            (result->limbs[1] == mf->p.limbs[1] && result->limbs[0] >= mf->p.limbs[0])))))) {
        
        uint64_t borrow = 0;
        uint64_t diff0 = result->limbs[0] - mf->p.limbs[0];
        uint64_t diff1 = result->limbs[1] - mf->p.limbs[1] - (diff0 > result->limbs[0]);
        uint64_t diff2 = result->limbs[2] - mf->p.limbs[2] - (diff1 > result->limbs[1]);
        uint64_t diff3 = result->limbs[3] - mf->p.limbs[3] - (diff2 > result->limbs[2]);
        
        result->limbs[0] = diff0;
        result->limbs[1] = diff1;
        result->limbs[2] = diff2;
        result->limbs[3] = diff3;
    }
}
