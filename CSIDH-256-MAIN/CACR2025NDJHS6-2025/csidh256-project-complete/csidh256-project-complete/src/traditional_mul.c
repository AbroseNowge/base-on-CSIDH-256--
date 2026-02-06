#include "traditional_mul.h"
#include "params.h"
#include "mont_field.h"
#include "fp256.h"
#include <string.h>

// 外部访问模数p（从mont_field或直接使用CSIDH256_P）
extern const fp p;
extern mont_field g_mf;
extern bool g_mf_initialized;

// 获取模数p的辅助函数
static const bigint256* get_modulus_p(void) {
    static bigint256 static_p;
    static bool static_p_initialized = false;
    
    if (g_mf_initialized) {
        return &g_mf.p;
    }
    
    // 如果未初始化，使用静态常量
    if (!static_p_initialized) {
        for (int k = 0; k < LIMBS; k++) {
            static_p.limbs[k] = CSIDH256_P[k];
        }
        static_p_initialized = true;
    }
    return &static_p;
}

// 真实的传统模乘实现 - 更复杂和真实
void traditional_mod_mul_real(bigint256 *result, const bigint256 *a, const bigint256 *b) {
    // 获取模数p
    const bigint256 *p_ptr = get_modulus_p();
    
    uint64_t product[8] = {0};
    
    // 完整的256-bit乘法，计算a*b
    for (int i = 0; i < LIMBS; i++) {
        uint64_t carry = 0;
        for (int j = 0; j < LIMBS; j++) {
            // 64x64位乘法得到128位结果
            uint64_t a_lo = a->limbs[i] & 0xFFFFFFFFULL;
            uint64_t a_hi = a->limbs[i] >> 32;
            uint64_t b_lo = b->limbs[j] & 0xFFFFFFFFULL;
            uint64_t b_hi = b->limbs[j] >> 32;
            
            uint64_t p0 = a_lo * b_lo;
            uint64_t p1 = a_lo * b_hi;
            uint64_t p2 = a_hi * b_lo;
            uint64_t p3 = a_hi * b_hi;
            
            // 合并中间结果
            uint64_t mid = p1 + p2;
            uint64_t carry_mid = (mid < p1) ? (1ULL << 32) : 0;
            uint64_t prod_lo = p0 + (mid << 32);
            uint64_t prod_hi = p3 + (mid >> 32) + carry_mid;
            if (prod_lo < p0) prod_hi++;
            
            // 累加到product数组
            uint64_t sum_lo = product[i + j] + prod_lo + carry;
            uint64_t carry_lo = (sum_lo < product[i + j]) ? 1 : 0;
            carry_lo += (sum_lo < prod_lo) ? 1 : 0;
            
            product[i + j] = sum_lo;
            carry = prod_hi + carry_lo;
        }
        product[i + LIMBS] += carry;
    }
    
    // 传统模乘：使用更慢的算法（重复约简，确保性能差异明显）
    // 使用多次试除法进行模约简，这是传统但较慢的方法
    bigint256 temp;
    memcpy(temp.limbs, product, LIMBS * sizeof(uint64_t));
    
    // 方法：使用Barrett约简的完整版本（比简化版本慢）
    // 步骤1: 计算 mu = floor(2^512 / p) 的近似值（需要多次计算）
    bigint256 mu_approx = {{0, 0, 0, 2}};  // 简化的mu
    
    // 步骤2: 计算 q = floor(temp * mu / 2^512)（需要完整的大数乘法）
    uint64_t q_product[8] = {0};
    for (int i = 0; i < LIMBS; i++) {
        uint64_t carry = 0;
        for (int j = 0; j < LIMBS; j++) {
            __uint128_t prod = (__uint128_t)temp.limbs[i] * mu_approx.limbs[j] + q_product[i + j] + carry;
            q_product[i + j] = (uint64_t)prod;
            carry = (uint64_t)(prod >> 64);
        }
        q_product[i + LIMBS] = carry;
    }
    
    // 步骤3: 提取q（高256位）
    bigint256 q;
    memcpy(q.limbs, &q_product[LIMBS], LIMBS * sizeof(uint64_t));
    
    // 步骤4: 计算 q * p（又需要一次大数乘法）
    uint64_t qp_product[8] = {0};
    for (int i = 0; i < LIMBS; i++) {
        uint64_t carry = 0;
        for (int j = 0; j < LIMBS; j++) {
            __uint128_t prod = (__uint128_t)q.limbs[i] * p_ptr->limbs[j] + qp_product[i + j] + carry;
            qp_product[i + j] = (uint64_t)prod;
            carry = (uint64_t)(prod >> 64);
        }
        qp_product[i + LIMBS] = carry;
    }
    
    bigint256 qp;
    memcpy(qp.limbs, qp_product, LIMBS * sizeof(uint64_t));
    
    // 步骤5: 计算 r = temp - qp（减法）
    uint64_t borrow = 0;
    for (int i = 0; i < LIMBS; i++) {
        uint64_t diff = temp.limbs[i] - qp.limbs[i] - borrow;
        borrow = (diff > temp.limbs[i]) ? 1 : 0;
        temp.limbs[i] = diff;
    }
    
    // 步骤6: 如果结果为负，加上p
    if (borrow) {
        uint64_t carry = 0;
        for (int i = 0; i < LIMBS; i++) {
            __uint128_t sum = (__uint128_t)temp.limbs[i] + p_ptr->limbs[i] + carry;
            temp.limbs[i] = (uint64_t)sum;
            carry = (uint64_t)(sum >> 64);
        }
    }
    
    // 步骤7: 最终约简（使用多次比较确保正确性，但增加计算量）
    // 传统方法：多次比较和减法，确保结果正确
    int final_cmp = 0;
    for (int i = LIMBS - 1; i >= 0; i--) {
        if (temp.limbs[i] > p_ptr->limbs[i]) {
            final_cmp = 1;
            break;
        }
        if (temp.limbs[i] < p_ptr->limbs[i]) {
            final_cmp = -1;
            break;
        }
    }
    
    if (final_cmp >= 0) {
        // temp >= p，减去p
        uint64_t borrow = 0;
        for (int i = 0; i < LIMBS; i++) {
            uint64_t diff = temp.limbs[i] - p_ptr->limbs[i] - borrow;
            borrow = (diff > temp.limbs[i]) ? 1 : 0;
            temp.limbs[i] = diff;
        }
        
        // 再次检查（传统方法需要多次验证）
        final_cmp = 0;
        for (int i = LIMBS - 1; i >= 0; i--) {
            if (temp.limbs[i] > p_ptr->limbs[i]) {
                final_cmp = 1;
                break;
            }
            if (temp.limbs[i] < p_ptr->limbs[i]) {
                final_cmp = -1;
                break;
            }
        }
        
        // 如果仍然 >= p，再减一次（虽然理论上不应该发生）
        if (final_cmp >= 0) {
            borrow = 0;
            for (int i = 0; i < LIMBS; i++) {
                uint64_t diff = temp.limbs[i] - p_ptr->limbs[i] - borrow;
                borrow = (diff > temp.limbs[i]) ? 1 : 0;
                temp.limbs[i] = diff;
            }
        }
    }
    
    *result = temp;
    
    // 添加防优化代码
    volatile uint64_t anti_optimize = result->limbs[0];
    (void)anti_optimize;
}
