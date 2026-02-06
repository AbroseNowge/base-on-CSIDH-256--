// 完整的CSIDH密钥交换演示程序
// 包含：密钥生成、公钥计算、序列化、密钥交换、验证一致性

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "src/fp256.h"
#include "src/edwards256.h"
#include "src/csidh256_params.h"
#include "src/param_validator.h"
#include "src/rng.h"

// 公钥序列化（将proj曲线序列化为字节数组）
void serialize_public_key(const proj pk, uint8_t *out, size_t *out_len) {
    // 每个fp是256位 = 32字节，proj包含2个fp = 64字节
    memcpy(out, &pk[0], 32);
    memcpy(out + 32, &pk[1], 32);
    *out_len = 64;
}

// 公钥反序列化（从字节数组恢复proj曲线）
void deserialize_public_key(proj pk, const uint8_t *in, size_t in_len) {
    if (in_len < 64) {
        fprintf(stderr, "Error: Invalid public key length\n");
        return;
    }
    memcpy(&pk[0], in, 32);
    memcpy(&pk[1], in + 32, 32);
}

// CSIDH密钥交换函数（确保初始化）
static uint8_t csidh_action(proj out, const uint8_t sk[], const proj in) {
    // 确保Montgomery域已初始化
    extern bool g_mf_initialized;
    extern void init_montgomery_field(void);
    extern void init_public_curve(void);
    
    if (!g_mf_initialized) {
        init_montgomery_field();
    }
    
    // 确保公共曲线E已初始化
    static bool E_initialized = false;
    if (!E_initialized) {
        init_public_curve();
        E_initialized = true;
    }
    
    // 重置计数器
    FP_ADD_COMPUTED = 0;
    FP_SQR_COMPUTED = 0;
    FP_MUL_COMPUTED = 0;
    
    // 验证输入曲线是否为超奇异曲线
    if (!validate(in)) {
        fprintf(stderr, "Error: Input curve is not supersingular\n");
        return 0;
    }
    
    // 执行CSIDH action
    action_evaluation(out, sk, in);
    
    return 1;
}

// 打印公钥（十六进制）
void print_public_key(const proj pk, const char *name) {
    printf("%s (a): ", name);
    for (int i = NUMBER_OF_WORDS - 1; i >= 0; i--) {
        printf("%016llx", (unsigned long long)pk[0].limbs[i]);
    }
    printf("\n");
    
    printf("%s (a-d): ", name);
    for (int i = NUMBER_OF_WORDS - 1; i >= 0; i--) {
        printf("%016llx", (unsigned long long)pk[1].limbs[i]);
    }
    printf("\n");
}

int main() {
    printf("=================================================================\n");
    printf("完整的CSIDH-256密钥交换演示\n");
    printf("=================================================================\n\n");
    
    // 1. 初始化
    printf("步骤1: 初始化系统...\n");
    printf("  - 验证参数...\n");
    bool params_valid = validate_csidh256_params();
    if (!params_valid) {
        printf("  警告: 参数验证失败，使用自定义参数\n");
    }
    
    printf("  - 初始化Montgomery域...\n");
    init_montgomery_field();
    
    printf("  - 初始化公共曲线E...\n");
    init_public_curve();
    printf("  初始化完成！\n\n");
    
    // 2. Alice生成密钥对
    printf("步骤2: Alice生成密钥对...\n");
    uint8_t sk_alice[N];
    random_key(sk_alice);
    printf_key(sk_alice, "sk_alice");
    
    proj pk_alice;
    if (!csidh_action(pk_alice, sk_alice, E)) {
        fprintf(stderr, "Error: Alice公钥计算失败\n");
        return 1;
    }
    printf("  Alice公钥计算成功\n");
    print_public_key(pk_alice, "pk_alice");
    printf("\n");
    
    // 3. Bob生成密钥对
    printf("步骤3: Bob生成密钥对...\n");
    uint8_t sk_bob[N];
    random_key(sk_bob);
    printf_key(sk_bob, "sk_bob");
    
    proj pk_bob;
    if (!csidh_action(pk_bob, sk_bob, E)) {
        fprintf(stderr, "Error: Bob公钥计算失败\n");
        return 1;
    }
    printf("  Bob公钥计算成功\n");
    print_public_key(pk_bob, "pk_bob");
    printf("\n");
    
    // 4. 序列化公钥（模拟网络传输）
    printf("步骤4: 序列化公钥（模拟网络传输）...\n");
    uint8_t pk_alice_serialized[64];
    uint8_t pk_bob_serialized[64];
    size_t len;
    
    serialize_public_key(pk_alice, pk_alice_serialized, &len);
    printf("  Alice公钥序列化: %zu 字节\n", len);
    
    serialize_public_key(pk_bob, pk_bob_serialized, &len);
    printf("  Bob公钥序列化: %zu 字节\n", len);
    printf("\n");
    
    // 5. 反序列化公钥（模拟接收）
    printf("步骤5: 反序列化公钥（模拟接收）...\n");
    proj pk_alice_received, pk_bob_received;
    deserialize_public_key(pk_alice_received, pk_alice_serialized, 64);
    deserialize_public_key(pk_bob_received, pk_bob_serialized, 64);
    printf("  公钥反序列化成功\n\n");
    
    // 6. 计算共享密钥
    printf("步骤6: 计算共享密钥...\n");
    proj ss_alice, ss_bob;
    
    printf("  Alice计算共享密钥: action(sk_alice, pk_bob)...\n");
    if (!csidh_action(ss_alice, sk_alice, pk_bob_received)) {
        fprintf(stderr, "Error: Alice共享密钥计算失败\n");
        return 1;
    }
    printf("    域运算: %lu M + %lu S + %lu A\n", 
           FP_MUL_COMPUTED, FP_SQR_COMPUTED, FP_ADD_COMPUTED);
    
    printf("  Bob计算共享密钥: action(sk_bob, pk_alice)...\n");
    if (!csidh_action(ss_bob, sk_bob, pk_alice_received)) {
        fprintf(stderr, "Error: Bob共享密钥计算失败\n");
        return 1;
    }
    printf("    域运算: %lu M + %lu S + %lu A\n", 
           FP_MUL_COMPUTED, FP_SQR_COMPUTED, FP_ADD_COMPUTED);
    printf("\n");
    
    // 7. 验证一致性
    printf("步骤7: 验证共享密钥一致性...\n");
    // 计算 a/(a-d) 进行比较
    fp ss_alice_ratio, ss_bob_ratio;
    fp ss_alice_inv, ss_bob_inv;
    
    fp_copy(&ss_alice_inv, &ss_alice[1]);
    fp_inv(&ss_alice_inv);
    fp_mul(&ss_alice_ratio, &ss_alice[0], &ss_alice_inv);
    
    fp_copy(&ss_bob_inv, &ss_bob[1]);
    fp_inv(&ss_bob_inv);
    fp_mul(&ss_bob_ratio, &ss_bob[0], &ss_bob_inv);
    
    if (fp_compare(&ss_alice_ratio, &ss_bob_ratio) == 0) {
        printf("  ✅ SUCCESS: 共享密钥匹配！\n");
        printf("  共享密钥比率: ");
        for (int i = NUMBER_OF_WORDS - 1; i >= 0; i--) {
            printf("%016llx", (unsigned long long)ss_alice_ratio.limbs[i]);
        }
        printf("\n");
    } else {
        printf("  ❌ ERROR: 共享密钥不匹配！\n");
        printf("  Alice比率: ");
        for (int i = NUMBER_OF_WORDS - 1; i >= 0; i--) {
            printf("%016llx", (unsigned long long)ss_alice_ratio.limbs[i]);
        }
        printf("\n");
        printf("  Bob比率: ");
        for (int i = NUMBER_OF_WORDS - 1; i >= 0; i--) {
            printf("%016llx", (unsigned long long)ss_bob_ratio.limbs[i]);
        }
        printf("\n");
        return 1;
    }
    
    printf("\n=================================================================\n");
    printf("完整的CSIDH密钥交换流程演示成功完成！\n");
    printf("=================================================================\n");
    
    return 0;
}


