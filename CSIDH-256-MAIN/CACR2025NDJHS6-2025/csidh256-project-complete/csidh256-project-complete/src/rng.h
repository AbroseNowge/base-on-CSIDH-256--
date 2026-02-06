#ifndef RNG_H
#define RNG_H

#include <stddef.h>
#include <stdint.h>

// 密码学安全的随机数生成
void randombytes(void *x, size_t l);

#endif // RNG_H


