#include "mont_field.h"
#include <string.h>

/* 优化版：减少进位链处理 */
static void mul64x64(uint64_t *hi, uint64_t *lo, uint64_t a, uint64_t b) {
    uint64_t a_lo = a & 0xFFFFFFFFULL;
    uint64_t a_hi = a >> 32;
    uint64_t b_lo = b & 0xFFFFFFFFULL;
    uint64_t b_hi = b >> 32;
    
    uint64_t p0 = a_lo * b_lo;
    uint64_t p1 = a_lo * b_hi;
    uint64_t p2 = a_hi * b_lo;
    uint64_t p3 = a_hi * b_hi;
    
    uint64_t mid = p1 + p2;
    uint64_t carry_mid = (mid < p1) ? (1ULL << 32) : 0;
    
    *lo = p0 + (mid << 32);
    *hi = p3 + (mid >> 32) + carry_mid + ((*lo < p0) ? 1 : 0);
}

/* 先声明 bigint_mul_512 函数 */
static void bigint_mul_512_optimized(uint64_t *result, const bigint256 *a, const bigint256 *b);

/* 优化版 Montgomery 约简：展开循环减少分支 */
static void mont_redc_optimized(bigint256 *result, const uint64_t *T, const mont_field *mf) {
    uint64_t temp[8];
    memcpy(temp, T, 8 * sizeof(uint64_t));
    
    /* 循环展开：处理所有 4 个 limb */
    for (int i = 0; i < LIMBS; i++) {
        uint64_t m_i = temp[i] * mf->p_inv.limbs[0];
        
        /* 内层循环展开 */
        uint64_t carry = 0;
        
        /* 处理 j=0 */
        {
            uint64_t prod_hi, prod_lo;
            mul64x64(&prod_hi, &prod_lo, m_i, mf->p.limbs[0]);
            
            uint64_t sum_lo = temp[i] + prod_lo;
            uint64_t carry_from_add = (sum_lo < temp[i]) ? 1 : 0;
            
            temp[i] = sum_lo + carry;
            carry = prod_hi + carry_from_add + ((temp[i] < sum_lo) ? 1 : 0);
        }
        
        /* 处理 j=1,2,3 */
        for (int j = 1; j < LIMBS; j++) {
            uint64_t prod_hi, prod_lo;
            mul64x64(&prod_hi, &prod_lo, m_i, mf->p.limbs[j]);
            
            uint64_t sum_lo = temp[i + j] + prod_lo;
            uint64_t carry_from_add = (sum_lo < temp[i + j]) ? 1 : 0;
            
            temp[i + j] = sum_lo + carry;
            carry = prod_hi + carry_from_add + ((temp[i + j] < sum_lo) ? 1 : 0);
        }
        
        /* 处理进位链：优化版只需 4 步 */
        int k = i + LIMBS;
        if (carry && k < 8) {
            temp[k] += carry;
            if (temp[k] == 0 && k + 1 < 8) {
                temp[k + 1] += 1;
            }
        }
    }
    
    memcpy(result->limbs, &temp[LIMBS], LIMBS * sizeof(uint64_t));
    
    /* 条件减法（无分支版本） */
    if (bigint_compare(result, &mf->p) >= 0) {
        bigint256 temp_result;
        for (int i = 0; i < LIMBS; i++) {
            temp_result.limbs[i] = result->limbs[i] - mf->p.limbs[i];
        }
        memcpy(result, &temp_result, sizeof(bigint256));
    }
}

void mont_mul_optimized(bigint256 *result, const bigint256 *a, const bigint256 *b, 
                        const mont_field *mf) {
    uint64_t T[8];
    bigint_mul_512_optimized(T, a, b);
    mont_redc_optimized(result, T, mf);
}

/* 现在定义 bigint_mul_512_optimized */
static void bigint_mul_512_optimized(uint64_t *result, const bigint256 *a, const bigint256 *b) {
    int i, j;
    
    memset(result, 0, 8 * sizeof(uint64_t));
    
    for (i = 0; i < LIMBS; i++) {
        uint64_t carry = 0;
        for (j = 0; j < LIMBS; j++) {
            uint64_t prod_hi, prod_lo;
            mul64x64(&prod_hi, &prod_lo, a->limbs[i], b->limbs[j]);
            
            uint64_t sum_lo = result[i + j] + prod_lo;
            uint64_t carry_from_add = (sum_lo < result[i + j]) ? 1 : 0;
            
            result[i + j] = sum_lo + carry;
            carry = prod_hi + carry_from_add + ((result[i + j] < sum_lo) ? 1 : 0);
        }
        result[i + LIMBS] += carry;
    }
}

static uint64_t inv_mod_2_64_optimized(uint64_t a) {
    uint64_t x = a;
    int i;
    
    for (i = 0; i < 6; i++) {
        x = x * (2ULL - a * x);
    }
    return x;
}

void mont_field_init_optimized(mont_field *mf) {
    int i;
    bigint256 R;
    
    // 使用 params.h 中的 CSIDH256_P
    memcpy(mf->p.limbs, CSIDH256_P, LIMBS * sizeof(uint64_t));
    
    mf->p_inv.limbs[0] = 0ULL - inv_mod_2_64_optimized(mf->p.limbs[0]);
    for (i = 1; i < LIMBS; i++) {
        mf->p_inv.limbs[i] = 0;
    }
    
    memset(&R, 0, sizeof(R));
    R.limbs[0] = 1;
    
    for (i = 0; i < 256; i++) {
        bigint_add(&R, &R, &R);
        if (bigint_compare(&R, &mf->p) >= 0) {
            bigint_sub(&R, &R, &mf->p, &mf->p);
        }
    }
    
    mont_mul_optimized(&mf->r_squared, &R, &R, mf);
}

void to_mont_optimized(bigint256 *result, const bigint256 *a, const mont_field *mf) {
    mont_mul_optimized(result, a, &mf->r_squared, mf);
}

void from_mont_optimized(bigint256 *result, const bigint256 *a, const mont_field *mf) {
    uint64_t T[8];
    memset(T, 0, 8 * sizeof(uint64_t));
    memcpy(T, a->limbs, LIMBS * sizeof(uint64_t));
    mont_redc_optimized(result, T, mf);
}
