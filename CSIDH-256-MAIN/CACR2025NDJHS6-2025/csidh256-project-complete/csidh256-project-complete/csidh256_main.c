#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include "src/fp256.h"
#include "src/edwards256.h"
#include "src/csidh256_params.h"
#include "src/param_validator.h"

// 测量性能
static uint64_t get_cycles() {
#ifdef _WIN32
    return __rdtsc();
#else
    uint32_t lo, hi;
    asm volatile("rdtsc":"=a"(lo),"=d"(hi));
    return ((uint64_t)hi << 32) | lo;
#endif
}

static uint8_t csidh(proj out, const uint8_t sk[], const proj in) {
    // 确保Montgomery域已初始化（必须在任何域运算之前）
    extern bool g_mf_initialized;
    extern void init_montgomery_field(void);
    extern void init_public_curve(void);
    
    if (!g_mf_initialized) {
        init_montgomery_field();
    }
    
    // 确保公共曲线E已初始化
    extern proj E;
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
        return 0;
    }
    
    // 执行CSIDH action：计算群作用
    // out = action(sk, in)，即对曲线in应用密钥sk对应的同源
    action_evaluation(out, sk, in);
    
    return 1;
}

int main() {
    uint64_t c0, c1;
    
    fp u;
    uint8_t sk_alice[N], sk_bob[N];
    
    // 初始化随机数生成器
    srand((unsigned int)time(NULL));
    
    // 验证CSIDH-256参数
    printf("=================================================================\n");
    printf("CSIDH-256 Parameter Validation\n");
    printf("=================================================================\n");
    bool params_valid = validate_csidh256_params();
    if (!params_valid) {
        printf("WARNING: Some parameters may not be correct!\n");
        printf("The implementation may not work correctly.\n");
    }
    printf("\n");
    
    // 初始化Montgomery域
    printf("Initializing Montgomery field...\n");
    init_montgomery_field();
    printf("Montgomery field initialized.\n\n");
    
    // 初始化公共曲线E
    printf("Initializing public curve E...\n");
    init_public_curve();
    printf("Public curve E initialized.\n\n");
    
    // 测试随机密钥生成和CSIDH计算
    proj random_E, tmp_E;
    uint8_t key[N];
    random_key(key);
    csidh(random_E, key, E);
    
    printf("NOTE: all the arithmetic is in Montgomery domain. ");
    printf("In addition, the ordering of the prime factors l_i's is:\n");
    printf("L := {\t  %3d", L[0]);
    
    for (int i = 1; i < N; i++) {
        printf(", %3d", L[i]);
        if ((i % 18) == 17) {
            printf("\n\t");
        }
    }
    printf("};\n");
    
    printf("Moreover, the secret exponents belong to { -B_i, ..., 0, ..., B_i } where\n");
    printf("B := {\t  %3d", B[0]);
    for (int i = 1; i < N; i++) {
        printf(", %3d", B[i]);
        if ((i % 18) == 17) {
            printf("\n\t");
        }
    }
    printf("};\n");
    printf("The dummy-free case implies that the secret exponents satisfy (e_i mod 2) = (b_i mod 2).\n");
    
    printf("-----------------------------------------------------------------------------------------------------\n");
    printf("First step: Alice and Bob random generate a secret key, and both compute and send their public curves\n\n");
    
    // Alice: 随机密钥生成
    random_key(sk_alice);
    printf_key(sk_alice, "sk_alice");
    
    proj E_alice;
    c0 = get_cycles();
    assert(csidh(E_alice, sk_alice, E));
    c1 = get_cycles();
    
    printf("E_alice computed\n");
    printf("clock cycles: %3.03lf\n", (1.0 * (c1 - c0)) / (1000000.0));
    printf("Number of field operations computed: (%lu)M + (%lu)S + (%lu)a\n\n", 
           FP_MUL_COMPUTED, FP_SQR_COMPUTED, FP_ADD_COMPUTED);
    
    // Bob: 随机密钥生成
    random_key(sk_bob);
    printf_key(sk_bob, "sk_bob");
    
    proj E_bob;
    c0 = get_cycles();
    assert(csidh(E_bob, sk_bob, E));
    c1 = get_cycles();
    
    printf("E_bob computed\n");
    printf("clock cycles: %3.03lf\n", (1.0 * (c1 - c0)) / (1000000.0));
    printf("Number of field operations computed: (%lu)M + (%lu)S + (%lu)a\n", 
           FP_MUL_COMPUTED, FP_SQR_COMPUTED, FP_ADD_COMPUTED);
    
    printf("\n");
    printf("------------------------------------------------------------------------------------------------------------\n");
    printf("Second step: Alice and Bob compute the shared secret by using the public curves of Bob and Alice, respectively\n");
    
    // Alice: 共享密钥
    proj ss_alice;
    c0 = get_cycles();
    assert(csidh(ss_alice, sk_alice, E_bob));
    c1 = get_cycles();
    
    printf("ss_alice computed\n");
    printf("clock cycles: %3.03lf\n", (1.0 * (c1 - c0)) / (1000000.0));
    printf("Number of field operations computed: (%lu)M + (%lu)S + (%lu)a\n", 
           FP_MUL_COMPUTED, FP_SQR_COMPUTED, FP_ADD_COMPUTED);
    
    printf("\n");
    // Bob: 共享密钥
    proj ss_bob;
    c0 = get_cycles();
    assert(csidh(ss_bob, sk_bob, E_alice));
    c1 = get_cycles();
    
    printf("ss_bob computed\n");
    printf("clock cycles: %3.03lf\n", (1.0 * (c1 - c0)) / (1000000.0));
    printf("Number of field operations computed: (%lu)M + (%lu)S + (%lu)a\n", 
           FP_MUL_COMPUTED, FP_SQR_COMPUTED, FP_ADD_COMPUTED);
    
    printf("\n");
    printf("------------------------------------------------------------------------------------------------------------\n");
    printf("At the end of the protocol, Alice and Bob have different but isomorphic Edwards curves. In other words, the\n");
    printf("Montgomery curve isomorphic to each one is the same. Thus, (ss_alice_a / ss_alice_ad) = (ss_bob_a / ss_bob_ad).\n");
    
    fp ss_a, ss_b, ss_alice_inv, ss_bob_inv;
    fp_copy(&ss_alice_inv, &ss_alice[1]);
    fp_inv(&ss_alice_inv);
    fp_mul(&ss_a, &ss_alice[0], &ss_alice_inv);
    
    fp_copy(&ss_bob_inv, &ss_bob[1]);
    fp_inv(&ss_bob_inv);
    fp_mul(&ss_b, &ss_bob[0], &ss_bob_inv);
    
    // 验证共享密钥是否相同
    if (fp_compare(&ss_a, &ss_b) == 0) {
        printf("\x1b[32mSUCCESS: Shared secrets match!\x1b[0m\n");
    } else {
        printf("\x1b[31mERROR: Shared secrets do not match!\x1b[0m\n");
    }
    printf("\n");
    
    return 0;
}

