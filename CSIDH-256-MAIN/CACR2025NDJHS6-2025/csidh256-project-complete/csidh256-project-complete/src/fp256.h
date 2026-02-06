#ifndef FP256_H
#define FP256_H

#include "params.h"
#include "mont_field.h"
#include "csidh256_params.h"
#include <stdint.h>
#include <stdbool.h>

// 256位域元素类型（在Montgomery域中）
typedef bigint256 fp;

// 全局Montgomery域结构（需要在实现文件中定义）
extern mont_field g_mf;
extern bool g_mf_initialized;
extern const fp p;
extern fp R_mod_p;
extern fp R_squared_mod_p;
extern fp p_minus_1_halves;

// 初始化函数
void init_montgomery_field(void);

// 域运算计数器（用于性能分析）
extern uint64_t FP_ADD_COMPUTED;
extern uint64_t FP_SQR_COMPUTED;
extern uint64_t FP_MUL_COMPUTED;

// 模乘方法选择函数
void set_mul_method(int method);  // 0=传统模乘, 1=Montgomery模乘
int get_mul_method(void);

// 域运算函数（适配现有的Montgomery实现）
void fp_cswap(fp *x, fp *y, uint8_t c);

void fp_add(fp *c, const fp *a, const fp *b);
void fp_sub(fp *c, const fp *a, const fp *b);
void fp_mul(fp *c, const fp *a, const fp *b);
void fp_sqr(fp *b, const fp *a);

void fp_inv(fp *x);
uint8_t fp_issquare(const fp *x);
void fp_random(fp *x);

// 辅助函数（使用指针传递）
static inline void set_zero(fp *x) {
    for (int i = 0; i < NUMBER_OF_WORDS; i++) {
        x->limbs[i] = 0;
    }
}

static inline void set_one(fp *x) {
    x->limbs[0] = 1;
    for (int i = 1; i < NUMBER_OF_WORDS; i++) {
        x->limbs[i] = 0;
    }
}

static inline void fp_copy(fp *x, const fp *y) {
    for (int i = 0; i < NUMBER_OF_WORDS; i++) {
        x->limbs[i] = y->limbs[i];
    }
}

static inline int fp_compare(const fp *x, const fp *y) {
    for (int i = NUMBER_OF_WORDS - 1; i >= 0; i--) {
        if (x->limbs[i] > y->limbs[i]) return 1;
        if (x->limbs[i] < y->limbs[i]) return -1;
    }
    return 0;
}

static inline int fp_iszero(const fp *x) {
    for (int i = 0; i < NUMBER_OF_WORDS; i++) {
        if (x->limbs[i] != 0) return 0;
    }
    return 1;
}

// 常量时间比较
static inline void cmov(int8_t *r, const int8_t a, uint32_t b) {
    uint32_t t;
    b = -b;
    t = (*r ^ a) & b;
    *r ^= t;
}

static inline uint32_t isequal(uint32_t a, uint32_t b) {
    uint32_t r = 0;
    unsigned char *ta = (unsigned char *)&a;
    unsigned char *tb = (unsigned char *)&b;
    r = (ta[0] ^ tb[0]) | (ta[1] ^ tb[1]) | (ta[2] ^ tb[2]) | (ta[3] ^ tb[3]);
    r = (-r);
    r = r >> 31;
    return (uint32_t)(1 - r);
}

static inline int32_t issmaller(int32_t x, int32_t y) {
    int32_t xy = x ^ y;
    int32_t c = x - y;
    c ^= xy & (c ^ x);
    return (c >> 31);
}

#endif // FP256_H

