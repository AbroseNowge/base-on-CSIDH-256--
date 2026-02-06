#ifndef PARAM_VALIDATOR_H
#define PARAM_VALIDATOR_H

#include <stdbool.h>
#include <stdint.h>

// 全局变量：参数验证状态
extern bool csidh_params_valid;

// 验证CSIDH-256参数
bool validate_csidh256_params(void);

// 计算正确的CSIDH-256素数
void compute_valid_csidh256_prime(uint64_t p[4]);

#endif // PARAM_VALIDATOR_H


