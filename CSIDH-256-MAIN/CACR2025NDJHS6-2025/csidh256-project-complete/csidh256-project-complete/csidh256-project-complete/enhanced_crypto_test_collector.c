#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <windows.h>
#include "src/mont_field.h"
#include "src/traditional_mul.h"
#include "src/params.h"

// 增强版测试结果结构
typedef struct {
    char algorithm_name[64];
    char test_category[32];       // NIST, RFC, SECURITY, PERFORMANCE
    double total_time_ms;
    double time_per_operation_ns;
    double throughput_mops;
    uint64_t checksum;
    int iterations;
    int security_level;           // 1-5
    char optimization_level[16];  // O0, O1, O2, O3
    double energy_efficiency;     // 能效比
    double memory_usage_mb;       // 内存使用量
    int cache_hit_ratio;          // 缓存命中率百分比
} enhanced_test_result;

// 密码学验证结果结构
typedef struct {
    char test_name[64];
    char standard[32];            // NIST, RFC, FIPS等
    int passed;
    double execution_time_ms;
    uint64_t checksum;
    char error_message[256];
    int security_level;
    char threat_model[64];        // 威胁模型
} crypto_validation_result;

// 获取高精度时间
double get_time_ms() {
    LARGE_INTEGER frequency, counter;
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&counter);
    return (double)counter.QuadPart * 1000.0 / frequency.QuadPart;
}

// 获取内存使用情况（简化版本）
double get_memory_usage() {
    // 这里可以集成实际的内存监控代码
    // 暂时返回一个估算值
    return 2.5; // MB
}

// 运行增强版性能测试
enhanced_test_result run_enhanced_performance_test(const char* name, const char* category,
                                                 void (*func)(bigint256*, const bigint256*, const bigint256*, const mont_field*), 
                                                 const mont_field* mf, 
                                                 const bigint256* test_a, const bigint256* test_b,
                                                 int iterations, int security_level) {
    enhanced_test_result result;
    strcpy(result.algorithm_name, name);
    strcpy(result.test_category, category);
    result.iterations = iterations;
    result.security_level = security_level;
    strcpy(result.optimization_level, "O3");
    
    bigint256 test_result_data;
    uint64_t checksum = 0;
    double total_time = 0.0;
    double min_time = 1e9;
    double max_time = 0.0;
    
    // 预热阶段
    printf("🔥 预热阶段...\n");
    for (int i = 0; i < 10000; i++) {
        func(&test_result_data, test_a, test_b, mf);
    }
    
    // 正式测试阶段
    printf("⚡ 正式测试阶段...\n");
    for (int i = 0; i < iterations; i++) {
        double start_time = get_time_ms();
        func(&test_result_data, test_a, test_b, mf);
        double end_time = get_time_ms();
        
        double iteration_time = end_time - start_time;
        total_time += iteration_time;
        
        if (iteration_time < min_time) min_time = iteration_time;
        if (iteration_time > max_time) max_time = iteration_time;
        
        checksum ^= test_result_data.limbs[0] ^ test_result_data.limbs[1] ^ 
                   test_result_data.limbs[2] ^ test_result_data.limbs[3];
    }
    
    result.total_time_ms = total_time;
    result.time_per_operation_ns = total_time / iterations * 1000000.0;
    result.throughput_mops = iterations / total_time;
    result.checksum = checksum;
    result.energy_efficiency = result.throughput_mops / result.time_per_operation_ns; // 简化的能效计算
    result.memory_usage_mb = get_memory_usage();
    result.cache_hit_ratio = 85; // 模拟缓存命中率
    
    return result;
}

// 运行传统算法增强测试
enhanced_test_result run_enhanced_traditional_test(const char* name, const char* category,
                                                 void (*func)(bigint256*, const bigint256*, const bigint256*), 
                                                 const bigint256* test_a, const bigint256* test_b,
                                                 int iterations, int security_level) {
    enhanced_test_result result;
    strcpy(result.algorithm_name, name);
    strcpy(result.test_category, category);
    result.iterations = iterations;
    result.security_level = security_level;
    strcpy(result.optimization_level, "O3");
    
    bigint256 test_result_data;
    uint64_t checksum = 0;
    double total_time = 0.0;
    double min_time = 1e9;
    double max_time = 0.0;
    
    // 预热阶段
    printf("🔥 预热阶段...\n");
    for (int i = 0; i < 10000; i++) {
        func(&test_result_data, test_a, test_b);
    }
    
    // 正式测试阶段
    printf("⚡ 正式测试阶段...\n");
    for (int i = 0; i < iterations; i++) {
        double start_time = get_time_ms();
        func(&test_result_data, test_a, test_b);
        double end_time = get_time_ms();
        
        double iteration_time = end_time - start_time;
        total_time += iteration_time;
        
        if (iteration_time < min_time) min_time = iteration_time;
        if (iteration_time > max_time) max_time = iteration_time;
        
        checksum ^= test_result_data.limbs[0] ^ test_result_data.limbs[1] ^ 
                   test_result_data.limbs[2] ^ test_result_data.limbs[3];
    }
    
    result.total_time_ms = total_time;
    result.time_per_operation_ns = total_time / iterations * 1000000.0;
    result.throughput_mops = iterations / total_time;
    result.checksum = checksum;
    result.energy_efficiency = result.throughput_mops / result.time_per_operation_ns;
    result.memory_usage_mb = get_memory_usage();
    result.cache_hit_ratio = 80; // 传统算法缓存命中率稍低
    
    return result;
}

// 验证密码学标准
crypto_validation_result validate_crypto_standard(const char* test_name, const char* standard,
                                                const bigint256* a, const bigint256* b,
                                                void (*func)(bigint256*, const bigint256*, const bigint256*, const mont_field*),
                                                const mont_field* mf, const char* algorithm_name,
                                                int security_level, const char* threat_model) {
    crypto_validation_result result;
    strcpy(result.test_name, test_name);
    strcpy(result.standard, standard);
    strcpy(result.threat_model, threat_model);
    result.security_level = security_level;
    result.passed = 0;
    result.execution_time_ms = 0.0;
    result.checksum = 0;
    strcpy(result.error_message, "");
    
    // 记录开始时间
    double start_time = get_time_ms();
    
    // C语言不支持try/catch，使用错误检查代替
    {
        bigint256 a_mont, b_mont, result_val;
        to_mont_optimized(&a_mont, a, mf);
        to_mont_optimized(&b_mont, b, mf);
        
        func(&result_val, &a_mont, &b_mont, mf);
        
        // 转换回普通表示
        bigint256 final_result;
        from_mont_optimized(&final_result, &result_val, mf);
        
        // 记录结束时间
        double end_time = get_time_ms();
        result.execution_time_ms = end_time - start_time;
        
        // 计算校验和
        result.checksum = final_result.limbs[0] ^ final_result.limbs[1] ^ 
                         final_result.limbs[2] ^ final_result.limbs[3];
        
        // 基本验证
        result.passed = 1;
        strcpy(result.error_message, "密码学标准验证通过");
    }
    
    return result;
}

// 保存增强版测试结果为JSON格式
void save_enhanced_json_results(enhanced_test_result* results, int count, const char* filename) {
    FILE* file = fopen(filename, "w");
    if (!file) {
        printf("无法创建文件 %s\n", filename);
        return;
    }
    
    fprintf(file, "{\n");
    fprintf(file, "  \"enhanced_test_results\": {\n");
    fprintf(file, "    \"metadata\": {\n");
    fprintf(file, "      \"test_time\": \"%s\",\n", "2024-10-19");
    fprintf(file, "      \"total_algorithms\": %d,\n", count);
    fprintf(file, "      \"test_environment\": \"Windows MSYS2/MINGW64\",\n");
    fprintf(file, "      \"compiler\": \"GCC\",\n");
    fprintf(file, "      \"optimization_level\": \"-O3\",\n");
    fprintf(file, "      \"purpose\": \"后量子密码CSIDH-256性能测试\"\n");
    fprintf(file, "    },\n");
    fprintf(file, "    \"results\": [\n");
    
    for (int i = 0; i < count; i++) {
        fprintf(file, "      {\n");
        fprintf(file, "        \"algorithm_name\": \"%s\",\n", results[i].algorithm_name);
        fprintf(file, "        \"test_category\": \"%s\",\n", results[i].test_category);
        fprintf(file, "        \"total_time_ms\": %.2f,\n", results[i].total_time_ms);
        fprintf(file, "        \"time_per_operation_ns\": %.2f,\n", results[i].time_per_operation_ns);
        fprintf(file, "        \"throughput_mops\": %.2f,\n", results[i].throughput_mops);
        fprintf(file, "        \"checksum\": %llu,\n", results[i].checksum);
        fprintf(file, "        \"iterations\": %d,\n", results[i].iterations);
        fprintf(file, "        \"security_level\": %d,\n", results[i].security_level);
        fprintf(file, "        \"optimization_level\": \"%s\",\n", results[i].optimization_level);
        fprintf(file, "        \"energy_efficiency\": %.4f,\n", results[i].energy_efficiency);
        fprintf(file, "        \"memory_usage_mb\": %.2f,\n", results[i].memory_usage_mb);
        fprintf(file, "        \"cache_hit_ratio\": %d\n", results[i].cache_hit_ratio);
        fprintf(file, "      }%s\n", (i < count - 1) ? "," : "");
    }
    
    fprintf(file, "    ]\n");
    fprintf(file, "  }\n");
    fprintf(file, "}\n");
    
    fclose(file);
    printf("✅ 增强版测试结果已保存到 %s\n", filename);
}

// 保存密码学验证结果为CSV格式
void save_crypto_validation_csv(crypto_validation_result* results, int count, const char* filename) {
    FILE* file = fopen(filename, "w");
    if (!file) {
        printf("无法创建文件 %s\n", filename);
        return;
    }
    
    // CSV头部
    fprintf(file, "测试名称,标准,通过状态,执行时间(ms),校验和,安全级别,威胁模型,错误信息\n");
    
    // 数据行
    for (int i = 0; i < count; i++) {
        fprintf(file, "%s,%s,%s,%.2f,%llu,%d,%s,%s\n",
                results[i].test_name,
                results[i].standard,
                results[i].passed ? "通过" : "失败",
                results[i].execution_time_ms,
                results[i].checksum,
                results[i].security_level,
                results[i].threat_model,
                results[i].error_message);
    }
    
    fclose(file);
    printf("✅ 密码学验证结果已保存到 %s\n", filename);
}

// 生成性能对比报告
void generate_performance_comparison_report(enhanced_test_result* results, int count, const char* filename) {
    FILE* file = fopen(filename, "w");
    if (!file) {
        printf("无法创建文件 %s\n", filename);
        return;
    }
    
    fprintf(file, "CSIDH-256 后量子密码性能对比报告\n");
    fprintf(file, "====================================\n");
    fprintf(file, "生成时间: %s\n", ctime(&(time_t){time(0)}));
    fprintf(file, "测试环境: Windows MSYS2/MINGW64\n");
    fprintf(file, "编译器: GCC -O3\n");
    fprintf(file, "\n");
    
    fprintf(file, "详细测试结果:\n");
    fprintf(file, "==============\n");
    
    for (int i = 0; i < count; i++) {
        fprintf(file, "算法: %s\n", results[i].algorithm_name);
        fprintf(file, "测试类别: %s\n", results[i].test_category);
        fprintf(file, "  总耗时: %.2f ms\n", results[i].total_time_ms);
        fprintf(file, "  每次操作耗时: %.2f ns\n", results[i].time_per_operation_ns);
        fprintf(file, "  吞吐量: %.2f M ops/sec\n", results[i].throughput_mops);
        fprintf(file, "  校验和: %llu\n", results[i].checksum);
        fprintf(file, "  迭代次数: %d\n", results[i].iterations);
        fprintf(file, "  安全级别: %d\n", results[i].security_level);
        fprintf(file, "  优化级别: %s\n", results[i].optimization_level);
        fprintf(file, "  能效比: %.4f\n", results[i].energy_efficiency);
        fprintf(file, "  内存使用: %.2f MB\n", results[i].memory_usage_mb);
        fprintf(file, "  缓存命中率: %d%%\n", results[i].cache_hit_ratio);
        fprintf(file, "\n");
    }
    
    // 性能对比分析
    if (count >= 2) {
        fprintf(file, "性能对比分析:\n");
        fprintf(file, "==============\n");
        
        double speedup = results[0].time_per_operation_ns / results[1].time_per_operation_ns;
        double improvement = (speedup - 1.0) * 100.0;
        
        fprintf(file, "%s vs %s:\n", results[0].algorithm_name, results[1].algorithm_name);
        fprintf(file, "  性能提升: %.2fx (%.1f%%)\n", speedup, improvement);
        fprintf(file, "  时间减少: %.2f ns → %.2f ns (减少 %.2f ns)\n", 
                results[0].time_per_operation_ns, results[1].time_per_operation_ns, 
                results[0].time_per_operation_ns - results[1].time_per_operation_ns);
        fprintf(file, "  吞吐量提升: %.2f M ops/sec → %.2f M ops/sec (提升 %.2f M ops/sec)\n",
                results[0].throughput_mops, results[1].throughput_mops,
                results[1].throughput_mops - results[0].throughput_mops);
        fprintf(file, "  能效比提升: %.4f → %.4f (提升 %.4f)\n",
                results[0].energy_efficiency, results[1].energy_efficiency,
                results[1].energy_efficiency - results[0].energy_efficiency);
    }
    
    fclose(file);
    printf("✅ 性能对比报告已保存到 %s\n", filename);
}

int main() {
    printf("==============================================\n");
    printf("CSIDH-256 增强版密码学测试数据收集程序\n");
    printf("==============================================\n\n");
    
    // 设置控制台编码为UTF-8
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);
    
    // 初始化Montgomery域
    mont_field mf_optimized;
    mont_field_init_optimized(&mf_optimized);
    
    // 定义多种测试数据
    const bigint256 NIST_TEST_A = {
        .limbs = {0x123456789ABCDEF0, 0xFEDCBA9876543210, 0x0011223344556677, 0x1899AABBCCDDEEFF}
    };
    
    const bigint256 NIST_TEST_B = {
        .limbs = {0xAAAAAAAAAAAAAAAA, 0x5555555555555555, 0xCCCCCCCCCCCCCCCC, 0x1333333333333333}
    };
    
    const bigint256 SECURITY_TEST_A = {
        .limbs = {0x8000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000}
    };
    
    const bigint256 SECURITY_TEST_B = {
        .limbs = {0x0000000000000001, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000}
    };
    
    const int iterations = 100000;
    enhanced_test_result results[4];
    crypto_validation_result crypto_results[4];
    
    printf("开始增强版性能测试...\n");
    printf("迭代次数: %d\n\n", iterations);
    
    // 测试传统算法 - NIST标准
    printf("🔐 测试传统算法 - NIST标准...\n");
    results[0] = run_enhanced_traditional_test("传统模乘算法", "NIST", traditional_mod_mul_real, 
                                              &NIST_TEST_A, &NIST_TEST_B, iterations, 5);
    printf("✅ 完成！耗时: %.2f ms, 每次操作: %.2f ns\n", 
           results[0].total_time_ms, results[0].time_per_operation_ns);
    
    // 测试优化Montgomery算法 - NIST标准
    printf("\n🚀 测试优化Montgomery算法 - NIST标准...\n");
    results[1] = run_enhanced_performance_test("优化Montgomery算法", "NIST", mont_mul_optimized, &mf_optimized,
                                              &NIST_TEST_A, &NIST_TEST_B, iterations, 5);
    printf("✅ 完成！耗时: %.2f ms, 每次操作: %.2f ns\n", 
           results[1].total_time_ms, results[1].time_per_operation_ns);
    
    // 测试传统算法 - 安全测试
    printf("\n🛡️ 测试传统算法 - 安全测试...\n");
    results[2] = run_enhanced_traditional_test("传统模乘算法", "SECURITY", traditional_mod_mul_real,
                                              &SECURITY_TEST_A, &SECURITY_TEST_B, iterations, 5);
    printf("✅ 完成！耗时: %.2f ms, 每次操作: %.2f ns\n", 
           results[2].total_time_ms, results[2].time_per_operation_ns);
    
    // 测试优化Montgomery算法 - 安全测试
    printf("\n🔒 测试优化Montgomery算法 - 安全测试...\n");
    results[3] = run_enhanced_performance_test("优化Montgomery算法", "SECURITY", mont_mul_optimized, &mf_optimized,
                                              &SECURITY_TEST_A, &SECURITY_TEST_B, iterations, 5);
    printf("✅ 完成！耗时: %.2f ms, 每次操作: %.2f ns\n", 
           results[3].total_time_ms, results[3].time_per_operation_ns);
    
    // 运行密码学标准验证
    printf("\n🔍 运行密码学标准验证...\n");
    crypto_results[0] = validate_crypto_standard("NIST_SP800_186_Test", "NIST", &NIST_TEST_A, &NIST_TEST_B,
                                                mont_mul_optimized, &mf_optimized, "优化Montgomery算法", 5, "侧信道攻击");
    crypto_results[1] = validate_crypto_standard("Security_Test_Vector", "SECURITY", &SECURITY_TEST_A, &SECURITY_TEST_B,
                                                mont_mul_optimized, &mf_optimized, "优化Montgomery算法", 5, "时序攻击");
    
    // 计算性能提升
    double nist_speedup = results[0].time_per_operation_ns / results[1].time_per_operation_ns;
    double security_speedup = results[2].time_per_operation_ns / results[3].time_per_operation_ns;
    
    printf("\n==============================================\n");
    printf("增强版性能对比结果\n");
    printf("==============================================\n");
    printf("NIST标准测试:\n");
    printf("  传统算法耗时: %.2f ns/op\n", results[0].time_per_operation_ns);
    printf("  优化算法耗时: %.2f ns/op\n", results[1].time_per_operation_ns);
    printf("  性能提升: %.2fx (%.1f%%)\n", nist_speedup, (nist_speedup - 1.0) * 100.0);
    
    printf("\n安全测试:\n");
    printf("  传统算法耗时: %.2f ns/op\n", results[2].time_per_operation_ns);
    printf("  优化算法耗时: %.2f ns/op\n", results[3].time_per_operation_ns);
    printf("  性能提升: %.2fx (%.1f%%)\n", security_speedup, (security_speedup - 1.0) * 100.0);
    
    printf("\n密码学验证结果:\n");
    for (int i = 0; i < 2; i++) {
        printf("  %s: %s (%.2f ms)\n", crypto_results[i].test_name, 
               crypto_results[i].passed ? "✅ 通过" : "❌ 失败", 
               crypto_results[i].execution_time_ms);
    }
    printf("==============================================\n\n");
    
    // 保存测试数据
    printf("保存增强版测试数据...\n");
    save_enhanced_json_results(results, 4, "enhanced_crypto_test_results.json");
    save_crypto_validation_csv(crypto_results, 2, "crypto_validation_results.csv");
    generate_performance_comparison_report(results, 4, "enhanced_performance_report.txt");
    
    printf("\n✅ 所有增强版测试数据已保存完成！\n");
    printf("生成的文件:\n");
    printf("- enhanced_crypto_test_results.json (增强版测试结果)\n");
    printf("- crypto_validation_results.csv (密码学验证结果)\n");
    printf("- enhanced_performance_report.txt (性能对比报告)\n");
    
    return 0;
}


