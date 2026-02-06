#ifndef EDWARDS256_H
#define EDWARDS256_H

#include "fp256.h"
#include "csidh256_params.h"
#include <stdint.h>

// 射影Edwards y坐标（用于CSIDH）
// proj[0] = a, proj[1] = (a - d)
// 注意：proj是数组类型，作为参数传递时自动转换为指针
typedef fp proj[2];

// 全局公共曲线E（同构于 y^2 = x^3 + x）
// 在实现文件中定义（非const，因为需要初始化）
extern proj E;

// 初始化公共曲线E
void init_public_curve(void);

// Edwards曲线运算
int isinfinity(const proj P);
void point_copy(proj Q, const proj P);
uint8_t areEqual(const proj P, const proj Q);

void yDBL(proj Q, const proj P, const proj A);
void yADD(proj R, const proj P, const proj Q, const proj PQ);
void yMUL(proj Q, const proj P, const proj A, uint8_t const i);

void elligator(proj T_plus, proj T_minus, const proj A);

void cofactor_multiples(proj P[], const proj A, int8_t lower, int8_t upper);
uint8_t validate(const proj A);

// 同源计算
void yISOG(proj Pk[], proj C, const proj P, const proj A, const uint8_t i);
void yEVAL(proj R, const proj Q, const proj Pk[], const uint8_t i);

// CSIDH action
void action_evaluation(proj C, const uint8_t key[], const proj A);
void random_key(uint8_t key[]);
void printf_key(uint8_t key[], char *c);

#endif // EDWARDS256_H

