#include "../src/params.h"
#include "../src/mont_field.h"  // 包含头文件以使用 get_time_ms
#include <stdio.h>
#include <stdlib.h>

/* 简单但真实的模乘实现（故意不优化） */
void traditional_mod_mul(bigint256 *result, const bigint256 *a, const bigint256 *b) {
    uint64_t product[8] = {0};
    int i, j;

    /* 64x64 -> 128 位乘法 */
    for (i = 0; i < LIMBS; i++) {
        uint64_t carry = 0;
        for (j = 0; j < LIMBS; j++) {
            uint64_t a_lo = a->limbs[i] & 0xFFFFFFFFULL;
            uint64_t a_hi = a->limbs[i] >> 32;
            uint64_t b_lo = b->limbs[j] & 0xFFFFFFFFULL;
            uint64_t b_hi = b->limbs[j] >> 32;

            uint64_t p0 = a_lo * b_lo;
            uint64_t p1 = a_lo * b_hi;
            uint64_t p2 = a_hi * b_lo;
            uint64_t p3 = a_hi * b_hi;

            uint64_t mid = p1 + p2;
            uint64_t carry_mid = (mid < p1) ? (1ULL << 32) : 0;
            uint64_t prod_lo = p0 + (mid << 32);
            uint64_t prod_hi = p3 + (mid >> 32) + carry_mid;
            if (prod_lo < p0) prod_hi++;

            uint64_t sum = product[i + j] + prod_lo + carry;
            product[i + j] = sum;
            carry = prod_hi + (sum < product[i + j] ? 1 : 0);
        }
        product[i + LIMBS] += carry;
    }

    /* 模约简：反复减去 p 直到 result < p（故意慢） */
    bigint256 temp;
    bigint256 p;
    int k;
    for (k = 0; k < LIMBS; k++) {
        temp.limbs[k] = product[k];
        p.limbs[k] = CSIDH256_P[k];
    }

    /* 最多循环 1000 次 */
    for (k = 0; k < 1000; k++) {
        int cmp = 0;
        for (j = LIMBS - 1; j >= 0; j--) {
            if (temp.limbs[j] > p.limbs[j]) {
                cmp = 1;
                break;
            }
            if (temp.limbs[j] < p.limbs[j]) {
                cmp = -1;
                break;
            }
        }
        if (cmp < 0) break;

        /* temp = temp - p */
        uint64_t borrow = 0;
        for (j = 0; j < LIMBS; j++) {
            uint64_t diff = temp.limbs[j] - p.limbs[j] - borrow;
            borrow = (diff > temp.limbs[j]) ? 1 : 0;
            temp.limbs[j] = diff;
        }
    }
    *result = temp;
}
