#include "edwards256.h"
#include "csidh256_params.h"
#include "rng.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

// 密钥生成
void random_key(uint8_t key[]) {
    uint8_t i, tmp, r;
    int8_t exp, sgn;
    
    for (i = 0; i < N; i++) {
        r = B[i] & 0x1;
        
        // 从 [0, B[i]] 中随机选择exp（使用密码学安全的RNG）
        extern void randombytes(void *x, size_t l);
        randombytes(&tmp, 1);
        tmp = tmp % (B[i] + 1);
        while (issmaller((int32_t)B[i], (int32_t)tmp) == -1) {
            randombytes(&tmp, 1);
            tmp = tmp % (B[i] + 1);
        }
        
        exp = (int8_t)tmp;
        
        // 映射到 [-B/2, B/2] 或 [-(B+1)/2, (B-1)/2]
        exp = ((exp << 1) - (B[i] + r)) >> 1;
        
        // 映射到 [-B, B]
        exp = (exp << 1) + r;
        sgn = exp >> 7;
        
        cmov(&exp, -exp, sgn == -1);
        key[i] = (exp << 1) ^ (1 & (1 + sgn));
    }
}

// 打印密钥
void printf_key(uint8_t key[], char *c) {
    int i;
    printf("%s := ", c);
    printf("{\t  %3d", (int)((2 * (key[0] & 0x1) - 1) * (key[0] >> 1)));
    
    for (i = 1; i < N; i++) {
        printf(", %3d", (int)((2 * (key[i] & 0x1) - 1) * (key[i] >> 1)));
        if ((i % 18) == 17) {
            printf("\n\t\t");
        }
    }
    printf("};\n");
}

// 常量时间查找
static uint32_t lookup(size_t pos, int8_t const priv[]) {
    int b;
    int8_t r = priv[0];
    for (size_t i = 1; i < N; i++) {
        b = isequal(i, pos);
        cmov(&r, priv[i], b);
    }
    return r;
}

// CSIDH action evaluation (SIMBA算法)
void action_evaluation(proj C, const uint8_t key[], const proj A) {
    // SIMBA参数
    uint8_t batches[NUMBER_OF_BATCHES][SIZE_OF_EACH_BATCH[0]];
    uint8_t size_of_each_batch[NUMBER_OF_BATCHES];
    
    for (uint8_t i = 0; i < NUMBER_OF_BATCHES; i++) {
        memcpy(batches[i], BATCHES[i], sizeof(uint8_t) * SIZE_OF_EACH_BATCH[i]);
    }
    
    memcpy(size_of_each_batch, SIZE_OF_EACH_BATCH, sizeof(uint8_t) * NUMBER_OF_BATCHES);
    
    uint8_t complement_of_each_batch[NUMBER_OF_BATCHES][N];
    uint8_t size_of_each_complement_batch[NUMBER_OF_BATCHES];
    
    memcpy(complement_of_each_batch, COMPLEMENT_OF_EACH_BATCH, sizeof(uint8_t) * NUMBER_OF_BATCHES * N);
    memcpy(size_of_each_complement_batch, SIZE_OF_EACH_COMPLEMENT_BATCH, sizeof(uint8_t) * NUMBER_OF_BATCHES);
    
    // 复制公钥和私钥
    uint8_t tmp_e[N];
    memcpy(tmp_e, key, sizeof(uint8_t) * N);
    
    proj current_A, current_T[2];
    // 复制输入曲线A到current_A
    point_copy(current_A, A);
    
    // SIMBA变量
    int8_t ec = 0;
    uint16_t count = 0;
    proj G[2], K[(LARGE_L >> 1) + 1];
    uint8_t finished[N];
    memset(finished, 0, sizeof(uint8_t) * N);
    
    int8_t counter[N];
    memset(counter, 0, sizeof(int8_t) * N);
    memcpy(counter, B, sizeof(int8_t) * N);
    uint64_t isog_counter = 0;
    
    uint8_t last_isogeny[NUMBER_OF_BATCHES];
    memcpy(last_isogeny, LAST_ISOGENY, sizeof(uint8_t) * NUMBER_OF_BATCHES);
    uint32_t bc;
    
    // 主循环
    uint8_t m = 0, i, j;
    uint64_t number_of_batches = NUMBER_OF_BATCHES;
    
    while (isog_counter < NUMBER_OF_ISOGENIES) {
        m = (m + 1) % number_of_batches;
        
        if (count == MY * number_of_batches) {
            m = 0;
            size_of_each_complement_batch[m] = 0;
            size_of_each_batch[m] = 0;
            number_of_batches = 1;
            
            for (i = 0; i < N; i++) {
                if (counter[i] == 0) {
                    complement_of_each_batch[m][size_of_each_complement_batch[m]] = i;
                    size_of_each_complement_batch[m] += 1;
                } else {
                    last_isogeny[0] = i;
                    batches[m][size_of_each_batch[m]] = i;
                    size_of_each_batch[m] += 1;
                }
            }
        }
        
        // 寻找合适的点
        elligator(current_T[1], current_T[0], current_A);
        
        // 乘以4和补集中的l_i
        yDBL(current_T[0], current_T[0], current_A);
        yDBL(current_T[0], current_T[0], current_A);
        yDBL(current_T[1], current_T[1], current_A);
        yDBL(current_T[1], current_T[1], current_A);
        
        for (i = 0; i < size_of_each_complement_batch[m]; i++) {
            yMUL(current_T[0], current_T[0], current_A, complement_of_each_batch[m][i]);
            yMUL(current_T[1], current_T[1], current_A, complement_of_each_batch[m][i]);
        }
        
        for (i = 0; i < size_of_each_batch[m]; i++) {
            if (finished[batches[m][i]] == 1) {
                continue;
            } else {
                point_copy(G[0], current_T[0]);
                point_copy(G[1], current_T[1]);
                
                ec = lookup(batches[m][i], tmp_e);
                fp_cswap(&G[0][0], &G[1][0], (ec & 1));
                fp_cswap(&G[0][1], &G[1][1], (ec & 1));
                
                fp_cswap(&current_T[0][0], &current_T[1][0], (ec & 1));
                fp_cswap(&current_T[0][1], &current_T[1][1], (ec & 1));
                
                for (j = (i + 1); j < size_of_each_batch[m]; j++) {
                    if (finished[batches[m][j]] == 0) {
                        yMUL(G[0], G[0], current_A, batches[m][j]);
                    }
                }
                
                if ((isinfinity(G[0]) != 1) && (isinfinity(G[1]) != 1)) {
                    bc = isequal(ec >> 1, 0) & 1;
                    
                    yISOG(K, current_A, G[0], current_A, batches[m][i]);
                    
                    if (isequal(batches[m][i], last_isogeny[m]) == 0) {
                        yEVAL(current_T[0], current_T[0], K, batches[m][i]);
                        yEVAL(current_T[1], current_T[1], K, batches[m][i]);
                        yMUL(current_T[1], current_T[1], current_A, batches[m][i]);
                    }
                    
                    tmp_e[batches[m][i]] = ((((ec >> 1) - (bc ^ 1)) ^ bc) << 1) ^ ((ec & 0x1) ^ bc);
                    counter[batches[m][i]] -= 1;
                    isog_counter += 1;
                } else {
                    yMUL(current_T[1], current_T[1], current_A, batches[m][i]);
                }
                
                fp_cswap(&current_T[0][0], &current_T[1][0], (ec & 1));
                fp_cswap(&current_T[0][1], &current_T[1][1], (ec & 1));
                
                if (counter[batches[m][i]] == 0) {
                    finished[batches[m][i]] = 1;
                    complement_of_each_batch[m][size_of_each_complement_batch[m]] = batches[m][i];
                    size_of_each_complement_batch[m] += 1;
                }
            }
        }
        count += 1;
    }
    
    // 将结果曲线复制到输出
    point_copy(C, current_A);
}

