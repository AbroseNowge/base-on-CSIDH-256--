#ifndef CSIDH256_PARAMS_H
#define CSIDH256_PARAMS_H

#include "params.h"
#include <stdint.h>

// ============================================================================
// 自定义CSIDH参数定义（非官方标准参数）
// ============================================================================
// 警告：此实现使用自定义参数集，不是官方CSIDH标准参数
// 
// 官方CSIDH-512使用74个小素数，CSIDH-256通常使用更少的素数
// 本实现使用37个小素数，这是为了适配256位素数p的自定义选择
//
// 安全性说明：
// - 此参数集的安全性未经过充分验证
// - 建议在生产环境使用前进行安全审计
// - 如需标准CSIDH参数，请参考官方CSIDH-512实现
// ============================================================================

#define N 37  // Number of small primes l_i such that l_i | [(p+1)/4]
#define NUMBER_OF_WORDS 4  // 256 / 64 = 4 words
#define LOG2_OF_N_PLUS_ONE 6

// 自定义小素数列表 L（37个，按降序排列）
// 这些是满足 l_i | (p+1)/4 的小素数
// 注意：这是自定义参数，不是官方CSIDH标准
static const uint32_t L[] = {
    163, 157, 151, 149, 139, 137, 131, 127,
    113, 109, 107, 103, 101,  97,  89,  83,
     79,  73,  71,  67,  61,  59,  53,  47,
     43,  41,  37,  31,  29,  23,  19,  17,
     13,  11,   7,   5,   3
};

// 自定义边界 B（每个l_i对应的最大指数）
// 这些值决定了密钥空间的大小
// 警告：这是自定义参数，使用统一的边界5
// 官方CSIDH参数使用变化的边界值（如7, 8, 11, 13等）
// 此统一边界可能影响安全性和性能，建议根据实际需求调整
static const int8_t B[] = {
    5,  5,  5,  5,  5,  5,  5,  5,
    5,  5,  5,  5,  5,  5,  5,  5,
    5,  5,  5,  5,  5,  5,  5,  5,
    5,  5,  5,  5,  5,  5,  5,  5,
    5,  5,  5,  5,  5
};

// 每个l_i的位数（用于验证）
static const uint16_t BITS_OF_L[] = {
    8, 8, 8, 8, 8, 8, 8, 7,
    7, 7, 7, 7, 7, 7, 7, 7,
    7, 7, 7, 7, 6, 6, 6, 6,
    6, 6, 6, 5, 5, 5, 5, 5,
    4, 4, 3, 3, 2
};

#define BITS_OF_4SQRT_OF_P 130  // 4*sqrt(p) 的位数（256位对应约130位）
#define LARGE_L 163  // 最大的l_i

// 最短差分加法链（用于标量乘法）
// 每个l_i对应一个加法链
static const uint64_t ADDITION_CHAIN[] = {
    0x0,   0x1,   0x1,   0x0,   0x4,   0x5,   0x0,   0x1,
    0x3,   0x8,   0xA,   0xD,   0x4,   0x14,  0x24,  0x9,
    0x2C,  0x32,  0x36,  0x11,  0x18,  0x4A,  0x60,  0xC,
    0x68,  0x49,  0x6C,  0x81,  0x82,  0x88,  0x8A,  0xC0,
    0xA1,  0xC2,  0x98,  0xD4,  0xC5
};

// 每个加法链的长度
static const uint8_t ADDITION_CHAIN_LENGTH[] = {
    0,  1,  1,  0,  2,  2,  0,  1,
    2,  3,  3,  3,  2,  4,  4,  3,
    4,  4,  4,  4,  4,  5,  5,  3,
    5,  5,  5,  6,  6,  6,  6,  6,
    4,  4,  3,  3,  2
};

// SIMBA参数（用于批处理同源计算）
#define NUMBER_OF_BATCHES 3
#define MY 8

// 批处理配置
static const uint8_t BATCH_0[] = { 0, 3, 6, 9, 12, 15, 18, 21, 24, 27, 30, 33, 36 };
static const uint8_t BATCH_1[] = { 1, 4, 7, 10, 13, 16, 19, 22, 25, 28, 31, 34 };
static const uint8_t BATCH_2[] = { 2, 5, 8, 11, 14, 17, 20, 23, 26, 29, 32, 35 };

static const uint8_t SIZE_OF_EACH_BATCH[NUMBER_OF_BATCHES] = {13, 12, 12};
static const uint8_t *BATCHES[NUMBER_OF_BATCHES] = { BATCH_0, BATCH_1, BATCH_2 };

static const uint8_t LAST_ISOGENY[NUMBER_OF_BATCHES] = { 36, 34, 35 };
static const uint16_t NUMBER_OF_ISOGENIES = 185;  // sum of all B[i]

// 每个批次的补集（不在该批次中的l_i）
static const uint8_t SIZE_OF_EACH_COMPLEMENT_BATCH[NUMBER_OF_BATCHES] = {24, 25, 25};
static const uint8_t COMPLEMENT_OF_EACH_BATCH[NUMBER_OF_BATCHES][N] = {
    // BATCH_0的补集
    { 1, 2, 4, 5, 7, 8, 10, 11, 13, 14, 16, 17, 19, 20, 22, 23, 25, 26, 28, 29, 31, 32, 34, 35,
      N, N, N, N, N, N, N, N, N, N, N, N, N },
    // BATCH_1的补集
    { 0, 2, 3, 5, 6, 8, 9, 11, 12, 14, 15, 17, 18, 20, 21, 23, 24, 26, 27, 29, 30, 32, 33, 35, 36,
      N, N, N, N, N, N, N, N, N, N, N, N },
    // BATCH_2的补集
    { 0, 1, 3, 4, 6, 7, 9, 10, 12, 13, 15, 16, 18, 19, 21, 22, 24, 25, 27, 28, 30, 31, 33, 34, 36,
      N, N, N, N, N, N, N, N, N, N, N, N }
};

#endif // CSIDH256_PARAMS_H

