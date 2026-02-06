#ifndef MONT_FIELD_H
#define MONT_FIELD_H

#include "params.h"
#include <stdbool.h>

// ==================== 基础版本函数声明 ====================

typedef struct {
    bigint256 p;           // 模数
    bigint256 p_inv;       // Montgomery 参数
    bigint256 r_squared;   // R^2 mod p
} mont_field;

// 初始化 Montgomery 域
void mont_field_init(mont_field *mf);

// Montgomery 乘法
void mont_mul(bigint256 *result, const bigint256 *a, const bigint256 *b, const mont_field *mf);

// 转换函数
void to_mont(bigint256 *result, const bigint256 *a, const mont_field *mf);
void from_mont(bigint256 *result, const bigint256 *a, const mont_field *mf);

// 辅助函数
void bigint_add(bigint256 *result, const bigint256 *a, const bigint256 *b);
void bigint_sub(bigint256 *result, const bigint256 *a, const bigint256 *b, const bigint256 *p);
int bigint_compare(const bigint256 *a, const bigint256 *b);

// ==================== 优化版本函数声明 ====================

// 优化版 Montgomery 乘法
void mont_mul_optimized(bigint256 *result, const bigint256 *a, const bigint256 *b, const mont_field *mf);

// 优化版初始化
void mont_field_init_optimized(mont_field *mf);

// 优化版转换函数
void to_mont_optimized(bigint256 *result, const bigint256 *a, const mont_field *mf);
void from_mont_optimized(bigint256 *result, const bigint256 *a, const mont_field *mf);

// ==================== 快速版本函数声明 ====================

// 快速 Montgomery 乘法
void mont_mul_fast(bigint256 *result, const bigint256 *a, const bigint256 *b, const mont_field *mf);

// 快速初始化
void mont_field_init_fast(mont_field *mf);

// 快速转换函数
void to_mont_fast(bigint256 *result, const bigint256 *a, const mont_field *mf);
void from_mont_fast(bigint256 *result, const bigint256 *a, const mont_field *mf);

// ==================== 超级优化版本函数声明 ====================

// 超级优化 Montgomery 乘法
void mont_mul_ultra(bigint256 *result, const bigint256 *a, const bigint256 *b, const mont_field *mf);

// 超级优化初始化
void mont_field_init_ultra(mont_field *mf);

// 超级优化转换函数
void to_mont_ultra(bigint256 *result, const bigint256 *a, const mont_field *mf);
void from_mont_ultra(bigint256 *result, const bigint256 *a, const mont_field *mf);

// ==================== SIMD优化版本函数声明 ====================

// SIMD优化 Montgomery 乘法
void mont_mul_simd(bigint256 *result, const bigint256 *a, const bigint256 *b, const mont_field *mf);

// ==================== 预计算表优化版本函数声明 ====================

// 预计算表优化 Montgomery 乘法
void mont_mul_precompute(bigint256 *result, const bigint256 *a, const bigint256 *b, const mont_field *mf);

// 初始化预计算表
void init_precompute_table(const mont_field *mf);

// ==================== 极限优化版本函数声明 ====================

// 极限优化Montgomery乘法
void mont_mul_extreme(bigint256 *result, const bigint256 *a, const bigint256 *b, const mont_field *mf);

// 极限优化初始化
void mont_field_init_extreme(mont_field *mf);

// 极限优化转换函数
void to_mont_extreme(bigint256 *result, const bigint256 *a, const mont_field *mf);
void from_mont_extreme(bigint256 *result, const bigint256 *a, const mont_field *mf);

// SIMD优化Montgomery乘法
void mont_mul_simd_avx2(bigint256 *result, const bigint256 *a, const bigint256 *b, const mont_field *mf);

// 并行优化Montgomery乘法
void mont_mul_parallel(bigint256 *result, const bigint256 *a, const bigint256 *b, const mont_field *mf);

// ==================== 高级优化版本函数声明 ====================

// 自适应Montgomery乘法
void adaptive_montgomery_mul(bigint256 *result, const bigint256 *a, const bigint256 *b, const mont_field *mf);

// 并行Montgomery乘法
void parallel_montgomery_mul(bigint256 *result, const bigint256 *a, const bigint256 *b, const mont_field *mf);

// 安全Montgomery乘法（抗侧信道攻击）
void secure_montgomery_mul(bigint256 *result, const bigint256 *a, const bigint256 *b, const mont_field *mf);

// 模式优化Montgomery乘法
void mont_mul_pattern_optimized(bigint256 *result, const bigint256 *a, const bigint256 *b, const mont_field *mf);

// ==================== 性能监控函数声明 ====================

// 性能指标结构
typedef struct {
    double avg_time;
    double max_time;
    double min_time;
    uint64_t total_operations;
    double throughput;
    double efficiency;
} performance_metrics;

// 性能监控函数
void monitor_performance_start(void);
void monitor_performance_update(double operation_time);
performance_metrics* get_performance_metrics(void);

// ==================== 实际应用函数声明 ====================

// CSIDH密钥对结构
typedef struct {
    bigint256 private_key;
    bigint256 public_key;
    mont_field mf;
} csidh_keypair;

// CSIDH密钥生成和交换
void generate_csidh_keypair(csidh_keypair *keypair);
void csidh_key_exchange(csidh_keypair *alice, csidh_keypair *bob, bigint256 *shared_secret);

// ==================== 性能测试辅助函数 ====================

// 获取当前时间（毫秒）
double get_time_ms(void);

// 验证函数
bool verify_montgomery_correctness(void);

#endif // MONT_FIELD_H
