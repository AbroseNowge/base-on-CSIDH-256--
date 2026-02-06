#include "fp256.h"
#include "csidh256_params.h"
#include "rng.h"
#include "traditional_mul.h"
#include <string.h>
#include <stdlib.h>

// 全局Montgomery域结构
mont_field g_mf;
bool g_mf_initialized = false;

// 域运算计数器
uint64_t FP_ADD_COMPUTED = 0;
uint64_t FP_SQR_COMPUTED = 0;
uint64_t FP_MUL_COMPUTED = 0;

// 模乘方法选择：0=传统模乘, 1=Montgomery模乘
int g_mul_method = 1;  // 默认使用Montgomery模乘

// 设置模乘方法
void set_mul_method(int method) {
    g_mul_method = (method == 0) ? 0 : 1;
}

// 获取当前模乘方法
int get_mul_method(void) {
    return g_mul_method;
}

// 素数p（CSIDH-256的素数）
// 注意：此素数用于演示目的，不进行参数验证

const fp p = { .limbs = {
    0xFFFFFFFFFFFFFFFF,
    0xFFFFFFFFFFFFFFFF,
    0xFFFFFFFFFFFFFFFF,
    0x1FFFFFFFFFFFFFFF
}};

// R = 2^256 mod p（需要在运行时计算）
fp R_mod_p;

// R^2 mod p（需要在运行时计算）
fp R_squared_mod_p;

// (p-1)/2（用于随机数生成）
fp p_minus_1_halves;

// 初始化Montgomery域
void init_montgomery_field(void) {
    if (g_mf_initialized) return;
    
    // 初始化Montgomery域结构（这会设置p和p_inv）
    mont_field_init(&g_mf);
    
    // 计算 R = 2^256 mod p（在普通域中）
    // R = 2^256，然后约简到 [0, p)
    bigint256 R_normal;
    set_zero(&R_normal);
    R_normal.limbs[0] = 1;
    
    // 计算 2^256 mod p（通过256次左移和约简）
    for (int i = 0; i < 256; i++) {
        bigint256 temp;
        bigint_add(&temp, &R_normal, &R_normal);
        if (bigint_compare(&temp, &g_mf.p) >= 0) {
            bigint_sub(&R_normal, &temp, &g_mf.p, &g_mf.p);
        } else {
            fp_copy(&R_normal, &temp);
        }
    }
    
    // R_mod_p = R（在普通域中，用于转换）
    fp_copy(&R_mod_p, &R_normal);
    
    // 计算 R^2 mod p（在普通域中，使用Montgomery乘法）
    // 注意：此时g_mf已经初始化，可以使用mont_mul
    bigint256 R_sq_normal;
    mont_mul(&R_sq_normal, &R_normal, &R_normal, &g_mf);
    fp_copy(&R_squared_mod_p, &R_sq_normal);
    
    // 计算 (p-1)/2（在普通域中）
    fp_copy(&p_minus_1_halves, (const fp*)&g_mf.p);
    uint64_t carry = 0;
    for (int i = NUMBER_OF_WORDS - 1; i >= 0; i--) {
        uint64_t new_carry = p_minus_1_halves.limbs[i] & 1;
        p_minus_1_halves.limbs[i] = (p_minus_1_halves.limbs[i] >> 1) | (carry << 63);
        carry = new_carry;
    }
    
    g_mf_initialized = true;
}

// 条件交换（常量时间）
void fp_cswap(fp *x, fp *y, uint8_t c) {
    uint64_t mask = -(uint64_t)c;
    for (int i = 0; i < NUMBER_OF_WORDS; i++) {
        uint64_t t = (x->limbs[i] ^ y->limbs[i]) & mask;
        x->limbs[i] ^= t;
        y->limbs[i] ^= t;
    }
}

// 域加法（在Montgomery域中）
void fp_add(fp *c, const fp *a, const fp *b) {
    if (!g_mf_initialized) init_montgomery_field();
    
    bigint256 temp;
    bigint_add(&temp, (const bigint256*)a, (const bigint256*)b);
    
    // 在Montgomery域中，需要检查是否超过p
    if (bigint_compare(&temp, &g_mf.p) >= 0) {
        bigint_sub((bigint256*)c, &temp, &g_mf.p, &g_mf.p);
    } else {
        fp_copy(c, &temp);
    }
    
    FP_ADD_COMPUTED++;
}

// 域减法（在Montgomery域中）
void fp_sub(fp *c, const fp *a, const fp *b) {
    if (!g_mf_initialized) init_montgomery_field();
    
    bigint256 temp;
    // 先计算a-b
    uint64_t borrow = 0;
    for (int i = 0; i < LIMBS; i++) {
        __uint128_t diff = (__uint128_t)a->limbs[i] - b->limbs[i] - borrow;
        temp.limbs[i] = (uint64_t)diff;
        borrow = (diff >> 64) ? 1 : 0;
    }
    
    // 如果借位，说明a < b，需要加p
    if (borrow) {
        bigint_add((bigint256*)c, &temp, &g_mf.p);
    } else {
        fp_copy(c, &temp);
    }
    
    FP_ADD_COMPUTED++;
}

// 域乘法（可切换：传统模乘或Montgomery模乘）
void fp_mul(fp *c, const fp *a, const fp *b) {
    if (g_mul_method == 0) {
        // 使用传统模乘
        traditional_mod_mul_real((bigint256*)c, (const bigint256*)a, (const bigint256*)b);
    } else {
        // 使用Montgomery模乘
        if (!g_mf_initialized) init_montgomery_field();
        mont_mul((bigint256*)c, (const bigint256*)a, (const bigint256*)b, &g_mf);
    }
    FP_MUL_COMPUTED++;
}

// 域平方（使用Montgomery乘法）
void fp_sqr(fp *b, const fp *a) {
    fp_mul(b, a, a);
    FP_SQR_COMPUTED++;
}

// 域逆元（使用扩展欧几里得算法或费马小定理）
void fp_inv(fp *x) {
    if (!g_mf_initialized) init_montgomery_field();
    
    // 使用费马小定理：x^(-1) = x^(p-2) mod p
    // 这里使用简单的二进制方法
    fp result, base;
    set_one(&result);
    fp_copy(&base, x);
    
    // p-2 = 2^256 - 3 (对于我们的p)
    bigint256 exp;
    fp_copy((fp*)&exp, (const fp*)&g_mf.p);
    exp.limbs[0] -= 2;
    
    for (int i = 0; i < 256; i++) {
        int word_idx = i / 64;
        int bit_idx = i % 64;
        if (word_idx < NUMBER_OF_WORDS && (exp.limbs[word_idx] >> bit_idx) & 1) {
            fp_mul(&result, &result, &base);
        }
        if (i < 255) {
            fp_sqr(&base, &base);
        }
    }
    
    fp_copy(x, &result);
    FP_MUL_COMPUTED += 256; // 近似计数
}

// 判断是否为平方数（Legendre符号）
uint8_t fp_issquare(const fp *x) {
    if (!g_mf_initialized) init_montgomery_field();
    
    // 使用欧拉准则：x是平方数当且仅当 x^((p-1)/2) = 1 mod p
    fp temp;
    fp_copy(&temp, x);
    
    // 计算 x^((p-1)/2)
    fp result;
    set_one(&result);
    fp_copy(&temp, x);
    
    bigint256 exp = p_minus_1_halves;
    
    for (int i = 0; i < 256; i++) {
        int word_idx = i / 64;
        int bit_idx = i % 64;
        if (word_idx < NUMBER_OF_WORDS && (exp.limbs[word_idx] >> bit_idx) & 1) {
            fp_mul(&result, &result, &temp);
        }
        if (i < 255) {
            fp_sqr(&temp, &temp);
        }
    }
    
    // 检查结果是否为1
    fp one;
    set_one(&one);
    return (fp_compare(&result, &one) == 0) ? 1 : 0;
}

// 生成随机域元素（在Montgomery域中）
void fp_random(fp *x) {
    // 确保Montgomery域已初始化
    if (!g_mf_initialized) {
        init_montgomery_field();
    }
    
    // 使用密码学安全的随机数生成器
    extern void randombytes(void *x, size_t l);
    
    // 生成随机数（在普通域中）
    bigint256 x_normal;
    do {
        // 使用密码学安全的RNG
        randombytes(x_normal.limbs, NUMBER_OF_WORDS * sizeof(uint64_t));
        
        // 确保最高位不超过p的最高位
        if (x_normal.limbs[NUMBER_OF_WORDS - 1] > g_mf.p.limbs[NUMBER_OF_WORDS - 1]) {
            x_normal.limbs[NUMBER_OF_WORDS - 1] = g_mf.p.limbs[NUMBER_OF_WORDS - 1];
        }
    } while (bigint_compare(&x_normal, &g_mf.p) >= 0);
    
    // 转换到Montgomery域：x_mont = x_normal * R^2 mod p
    mont_mul((bigint256*)x, &x_normal, &R_squared_mod_p, &g_mf);
}

