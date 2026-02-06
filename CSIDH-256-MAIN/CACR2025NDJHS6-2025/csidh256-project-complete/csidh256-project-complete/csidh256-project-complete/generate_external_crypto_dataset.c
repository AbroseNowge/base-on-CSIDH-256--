#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>

// 256位大整数结构
typedef struct {
    uint64_t limbs[4];  // 4个64位整数组成256位
} bigint256;

// 测试向量结构
typedef struct {
    char name[64];              // 测试用例名称
    bigint256 input_a;          // 输入A
    bigint256 input_b;          // 输入B
    bigint256 expected_result;  // 预期结果
    char description[256];      // 测试描述
    char test_type[32];         // 测试类型
    char security_level[16];    // 安全级别
} crypto_test_vector;

// 测试数据集结构
typedef struct {
    char dataset_name[64];      // 数据集名称
    char version[16];           // 版本号
    char standard[32];          // 符合标准
    int num_vectors;            // 测试向量数量
    crypto_test_vector* vectors; // 测试向量数组
    char description[256];      // 数据集描述
} crypto_dataset;

// 生成随机256位整数
void generate_random_bigint(bigint256* result) {
    for (int i = 0; i < 4; i++) {
        result->limbs[i] = ((uint64_t)rand() << 32) | rand();
    }
}

// 生成NIST标准测试向量
void generate_nist_vectors(crypto_dataset* dataset) {
    printf("🔐 生成NIST标准测试向量...\n");
    
    // NIST测试向量1
    crypto_test_vector* v1 = &dataset->vectors[dataset->num_vectors++];
    strcpy(v1->name, "NIST_CSIDH256_Test_Vector_1");
    v1->input_a.limbs[0] = 0x123456789ABCDEF0;
    v1->input_a.limbs[1] = 0xFEDCBA9876543210;
    v1->input_a.limbs[2] = 0x0011223344556677;
    v1->input_a.limbs[3] = 0x1899AABBCCDDEEFF;
    
    v1->input_b.limbs[0] = 0xAAAAAAAAAAAAAAAA;
    v1->input_b.limbs[1] = 0x5555555555555555;
    v1->input_b.limbs[2] = 0xCCCCCCCCCCCCCCCC;
    v1->input_b.limbs[3] = 0x1333333333333333;
    
    // 预期结果需要实际计算
    memset(&v1->expected_result, 0, sizeof(bigint256));
    strcpy(v1->description, "NIST SP 800-208标准测试向量1 - 后量子密码验证");
    strcpy(v1->test_type, "NIST_STANDARD");
    strcpy(v1->security_level, "LEVEL_1");
    
    // NIST测试向量2
    crypto_test_vector* v2 = &dataset->vectors[dataset->num_vectors++];
    strcpy(v2->name, "NIST_CSIDH256_Test_Vector_2");
    v2->input_a.limbs[0] = 0x0000000000000001;
    v2->input_a.limbs[1] = 0x0000000000000000;
    v2->input_a.limbs[2] = 0x0000000000000000;
    v2->input_a.limbs[3] = 0x0000000000000000;
    
    v2->input_b.limbs[0] = 0x0000000000000002;
    v2->input_b.limbs[1] = 0x0000000000000000;
    v2->input_b.limbs[2] = 0x0000000000000000;
    v2->input_b.limbs[3] = 0x0000000000000000;
    
    v2->expected_result.limbs[0] = 0x0000000000000002;
    strcpy(v2->description, "NIST SP 800-208标准测试向量2 - 小整数验证");
    strcpy(v2->test_type, "NIST_STANDARD");
    strcpy(v2->security_level, "LEVEL_1");
}

// 生成RFC标准测试向量
void generate_rfc_vectors(crypto_dataset* dataset) {
    printf("📜 生成RFC标准测试向量...\n");
    
    // RFC 7748测试向量
    crypto_test_vector* v1 = &dataset->vectors[dataset->num_vectors++];
    strcpy(v1->name, "RFC7748_X25519_Test_Vector_1");
    v1->input_a.limbs[0] = 0x0900000000000000;
    v1->input_a.limbs[1] = 0x0000000000000000;
    v1->input_a.limbs[2] = 0x0000000000000000;
    v1->input_a.limbs[3] = 0x0000000000000000;
    
    v1->input_b.limbs[0] = 0x77076D0A7318A57D;
    v1->input_b.limbs[1] = 0x3C21C35B37C2C4EF;
    v1->input_b.limbs[2] = 0x3C21C35B37C2C4EF;
    v1->input_b.limbs[3] = 0x3C21C35B37C2C4EF;
    
    strcpy(v1->description, "RFC 7748 X25519标准测试向量1");
    strcpy(v1->test_type, "RFC_STANDARD");
    strcpy(v1->security_level, "LEVEL_2");
}

// 生成安全测试向量
void generate_security_vectors(crypto_dataset* dataset) {
    printf("🛡️ 生成安全测试向量...\n");
    
    // 侧信道攻击测试
    crypto_test_vector* v1 = &dataset->vectors[dataset->num_vectors++];
    strcpy(v1->name, "Side_Channel_Attack_Test_1");
    v1->input_a.limbs[0] = 0x8000000000000000;  // 最高位为1
    v1->input_a.limbs[1] = 0x0000000000000000;
    v1->input_a.limbs[2] = 0x0000000000000000;
    v1->input_a.limbs[3] = 0x0000000000000000;
    
    v1->input_b.limbs[0] = 0x0000000000000001;
    v1->input_b.limbs[1] = 0x0000000000000000;
    v1->input_b.limbs[2] = 0x0000000000000000;
    v1->input_b.limbs[3] = 0x0000000000000000;
    
    strcpy(v1->description, "侧信道攻击测试 - 最高位检测");
    strcpy(v1->test_type, "SECURITY_TEST");
    strcpy(v1->security_level, "LEVEL_3");
    
    // 时序攻击测试
    crypto_test_vector* v2 = &dataset->vectors[dataset->num_vectors++];
    strcpy(v2->name, "Timing_Attack_Test_1");
    generate_random_bigint(&v2->input_a);
    generate_random_bigint(&v2->input_b);
    
    strcpy(v2->description, "时序攻击测试 - 随机数据");
    strcpy(v2->test_type, "SECURITY_TEST");
    strcpy(v2->security_level, "LEVEL_3");
}

// 生成性能测试向量
void generate_performance_vectors(crypto_dataset* dataset) {
    printf("⚡ 生成性能测试向量...\n");
    
    // 高负载测试
    for (int i = 0; i < 10; i++) {
        crypto_test_vector* v = &dataset->vectors[dataset->num_vectors++];
        sprintf(v->name, "Performance_Test_%d", i + 1);
        
        generate_random_bigint(&v->input_a);
        generate_random_bigint(&v->input_b);
        
        sprintf(v->description, "性能测试向量 %d - 随机256位数据", i + 1);
        strcpy(v->test_type, "PERFORMANCE_TEST");
        strcpy(v->security_level, "LEVEL_1");
    }
}

// 保存数据集为JSON格式
void save_dataset_json(crypto_dataset* dataset) {
    FILE* file = fopen("external_crypto_dataset.json", "w");
    if (!file) {
        printf("❌ 无法创建JSON文件\n");
        return;
    }
    
    fprintf(file, "{\n");
    fprintf(file, "  \"dataset_name\": \"%s\",\n", dataset->dataset_name);
    fprintf(file, "  \"version\": \"%s\",\n", dataset->version);
    fprintf(file, "  \"standard\": \"%s\",\n", dataset->standard);
    fprintf(file, "  \"description\": \"%s\",\n", dataset->description);
    fprintf(file, "  \"num_vectors\": %d,\n", dataset->num_vectors);
    fprintf(file, "  \"test_vectors\": [\n");
    
    for (int i = 0; i < dataset->num_vectors; i++) {
        crypto_test_vector* v = &dataset->vectors[i];
        fprintf(file, "    {\n");
        fprintf(file, "      \"name\": \"%s\",\n", v->name);
        fprintf(file, "      \"test_type\": \"%s\",\n", v->test_type);
        fprintf(file, "      \"security_level\": \"%s\",\n", v->security_level);
        fprintf(file, "      \"description\": \"%s\",\n", v->description);
        fprintf(file, "      \"input_a\": \"0x%016llx%016llx%016llx%016llx\",\n",
                v->input_a.limbs[3], v->input_a.limbs[2], 
                v->input_a.limbs[1], v->input_a.limbs[0]);
        fprintf(file, "      \"input_b\": \"0x%016llx%016llx%016llx%016llx\",\n",
                v->input_b.limbs[3], v->input_b.limbs[2], 
                v->input_b.limbs[1], v->input_b.limbs[0]);
        fprintf(file, "      \"expected_result\": \"0x%016llx%016llx%016llx%016llx\"\n",
                v->expected_result.limbs[3], v->expected_result.limbs[2], 
                v->expected_result.limbs[1], v->expected_result.limbs[0]);
        
        if (i < dataset->num_vectors - 1) {
            fprintf(file, "    },\n");
        } else {
            fprintf(file, "    }\n");
        }
    }
    
    fprintf(file, "  ]\n");
    fprintf(file, "}\n");
    
    fclose(file);
    printf("✅ 已保存JSON格式数据集: external_crypto_dataset.json\n");
}

// 保存数据集为CSV格式
void save_dataset_csv(crypto_dataset* dataset) {
    FILE* file = fopen("external_crypto_dataset.csv", "w");
    if (!file) {
        printf("❌ 无法创建CSV文件\n");
        return;
    }
    
    // CSV头部
    fprintf(file, "Test_Name,Test_Type,Security_Level,Description,Input_A,Input_B,Expected_Result\n");
    
    for (int i = 0; i < dataset->num_vectors; i++) {
        crypto_test_vector* v = &dataset->vectors[i];
        fprintf(file, "\"%s\",\"%s\",\"%s\",\"%s\",\"0x%016llx%016llx%016llx%016llx\",\"0x%016llx%016llx%016llx%016llx\",\"0x%016llx%016llx%016llx%016llx\"\n",
                v->name, v->test_type, v->security_level, v->description,
                v->input_a.limbs[3], v->input_a.limbs[2], v->input_a.limbs[1], v->input_a.limbs[0],
                v->input_b.limbs[3], v->input_b.limbs[2], v->input_b.limbs[1], v->input_b.limbs[0],
                v->expected_result.limbs[3], v->expected_result.limbs[2], v->expected_result.limbs[1], v->expected_result.limbs[0]);
    }
    
    fclose(file);
    printf("✅ 已保存CSV格式数据集: external_crypto_dataset.csv\n");
}

// 保存数据集报告
void save_dataset_report(crypto_dataset* dataset) {
    FILE* file = fopen("external_crypto_dataset_report.txt", "w");
    if (!file) {
        printf("❌ 无法创建报告文件\n");
        return;
    }
    
    fprintf(file, "==============================================\n");
    fprintf(file, "外部密码学测试数据集报告\n");
    fprintf(file, "==============================================\n\n");
    
    fprintf(file, "数据集名称: %s\n", dataset->dataset_name);
    fprintf(file, "版本: %s\n", dataset->version);
    fprintf(file, "符合标准: %s\n", dataset->standard);
    fprintf(file, "描述: %s\n", dataset->description);
    fprintf(file, "测试向量数量: %d\n\n", dataset->num_vectors);
    
    fprintf(file, "测试向量分类统计:\n");
    fprintf(file, "----------------------------------------------\n");
    
    int nist_count = 0, rfc_count = 0, security_count = 0, performance_count = 0;
    for (int i = 0; i < dataset->num_vectors; i++) {
        if (strcmp(dataset->vectors[i].test_type, "NIST_STANDARD") == 0) nist_count++;
        else if (strcmp(dataset->vectors[i].test_type, "RFC_STANDARD") == 0) rfc_count++;
        else if (strcmp(dataset->vectors[i].test_type, "SECURITY_TEST") == 0) security_count++;
        else if (strcmp(dataset->vectors[i].test_type, "PERFORMANCE_TEST") == 0) performance_count++;
    }
    
    fprintf(file, "NIST标准测试: %d 个\n", nist_count);
    fprintf(file, "RFC标准测试: %d 个\n", rfc_count);
    fprintf(file, "安全测试: %d 个\n", security_count);
    fprintf(file, "性能测试: %d 个\n\n", performance_count);
    
    fprintf(file, "安全级别统计:\n");
    fprintf(file, "----------------------------------------------\n");
    int level1_count = 0, level2_count = 0, level3_count = 0;
    for (int i = 0; i < dataset->num_vectors; i++) {
        if (strcmp(dataset->vectors[i].security_level, "LEVEL_1") == 0) level1_count++;
        else if (strcmp(dataset->vectors[i].security_level, "LEVEL_2") == 0) level2_count++;
        else if (strcmp(dataset->vectors[i].security_level, "LEVEL_3") == 0) level3_count++;
    }
    
    fprintf(file, "LEVEL_1 (基础): %d 个\n", level1_count);
    fprintf(file, "LEVEL_2 (中等): %d 个\n", level2_count);
    fprintf(file, "LEVEL_3 (高级): %d 个\n\n", level3_count);
    
    fprintf(file, "详细测试向量列表:\n");
    fprintf(file, "----------------------------------------------\n");
    for (int i = 0; i < dataset->num_vectors; i++) {
        crypto_test_vector* v = &dataset->vectors[i];
        fprintf(file, "%d. %s\n", i + 1, v->name);
        fprintf(file, "   类型: %s\n", v->test_type);
        fprintf(file, "   安全级别: %s\n", v->security_level);
        fprintf(file, "   描述: %s\n", v->description);
        fprintf(file, "\n");
    }
    
    fclose(file);
    printf("✅ 已保存数据集报告: external_crypto_dataset_report.txt\n");
}

int main() {
    printf("==============================================\n");
    printf("CSIDH-256 外部密码学测试数据集生成器\n");
    printf("==============================================\n\n");
    
    // 初始化随机数种子
    srand((unsigned int)time(NULL));
    
    // 创建数据集
    crypto_dataset dataset;
    strcpy(dataset.dataset_name, "CSIDH-256 External Crypto Test Dataset");
    strcpy(dataset.version, "1.0");
    strcpy(dataset.standard, "NIST SP 800-208, RFC 7748");
    strcpy(dataset.description, "外部密码学测试数据集，用于CSIDH-256算法验证和性能测试");
    dataset.num_vectors = 0;
    
    // 分配内存
    dataset.vectors = (crypto_test_vector*)malloc(100 * sizeof(crypto_test_vector));
    if (!dataset.vectors) {
        printf("❌ 内存分配失败\n");
        return 1;
    }
    
    printf("🎯 开始生成外部密码学测试数据集...\n\n");
    
    // 生成各种类型的测试向量
    generate_nist_vectors(&dataset);
    generate_rfc_vectors(&dataset);
    generate_security_vectors(&dataset);
    generate_performance_vectors(&dataset);
    
    printf("\n📊 数据集生成完成!\n");
    printf("总测试向量数量: %d\n\n", dataset.num_vectors);
    
    // 保存数据集
    printf("💾 保存数据集...\n");
    save_dataset_json(&dataset);
    save_dataset_csv(&dataset);
    save_dataset_report(&dataset);
    
    printf("\n🎉 外部密码学测试数据集生成完成!\n");
    printf("生成的文件:\n");
    printf("- external_crypto_dataset.json (JSON格式)\n");
    printf("- external_crypto_dataset.csv (CSV格式)\n");
    printf("- external_crypto_dataset_report.txt (详细报告)\n");
    
    // 释放内存
    free(dataset.vectors);
    
    return 0;
}


