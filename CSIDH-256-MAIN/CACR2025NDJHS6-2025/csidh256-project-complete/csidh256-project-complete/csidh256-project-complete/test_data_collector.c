#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <windows.h>
#include "src/mont_field.h"
#include "src/traditional_mul.h"
#include "src/params.h"

// 获取高精度时间
double get_time_ms() {
    LARGE_INTEGER frequency, counter;
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&counter);
    return (double)counter.QuadPart * 1000.0 / frequency.QuadPart;
}

// 测试数据
static const bigint256 TEST_A = {
    .limbs = {0x123456789ABCDEF0, 0xFEDCBA9876543210, 0x0011223344556677, 0x1899AABBCCDDEEFF}
};

static const bigint256 TEST_B = {
    .limbs = {0xAAAAAAAAAAAAAAAA, 0x5555555555555555, 0xCCCCCCCCCCCCCCCC, 0x1333333333333333}
};

// 测试结果结构
typedef struct {
    char algorithm_name[64];
    double total_time_ms;
    double time_per_operation_ns;
    double throughput_mops;
    uint64_t checksum;
    int iterations;
} test_result;

// 运行性能测试
test_result run_performance_test(const char* name, void (*func)(bigint256*, const bigint256*, const bigint256*, const mont_field*), 
                                const mont_field* mf, int iterations) {
    test_result result;
    strcpy(result.algorithm_name, name);
    result.iterations = iterations;
    
    bigint256 test_result_data;
    uint64_t checksum = 0;
    
    // 预热
    for (int i = 0; i < 1000; i++) {
        func(&test_result_data, &TEST_A, &TEST_B, mf);
    }
    
    // 正式测试
    double start_time = get_time_ms();
    
    for (int i = 0; i < iterations; i++) {
        func(&test_result_data, &TEST_A, &TEST_B, mf);
        checksum ^= test_result_data.limbs[0] ^ test_result_data.limbs[1] ^ test_result_data.limbs[2] ^ test_result_data.limbs[3];
    }
    
    double end_time = get_time_ms();
    
    result.total_time_ms = end_time - start_time;
    result.time_per_operation_ns = result.total_time_ms / iterations * 1000000.0;
    result.throughput_mops = iterations / result.total_time_ms;
    result.checksum = checksum;
    
    return result;
}

// 运行传统算法测试
test_result run_traditional_test(const char* name, void (*func)(bigint256*, const bigint256*, const bigint256*), int iterations) {
    test_result result;
    strcpy(result.algorithm_name, name);
    result.iterations = iterations;
    
    bigint256 test_result_data;
    uint64_t checksum = 0;
    
    // 预热
    for (int i = 0; i < 1000; i++) {
        func(&test_result_data, &TEST_A, &TEST_B);
    }
    
    // 正式测试
    double start_time = get_time_ms();
    
    for (int i = 0; i < iterations; i++) {
        func(&test_result_data, &TEST_A, &TEST_B);
        checksum ^= test_result_data.limbs[0] ^ test_result_data.limbs[1] ^ test_result_data.limbs[2] ^ test_result_data.limbs[3];
    }
    
    double end_time = get_time_ms();
    
    result.total_time_ms = end_time - start_time;
    result.time_per_operation_ns = result.total_time_ms / iterations * 1000000.0;
    result.throughput_mops = iterations / result.total_time_ms;
    result.checksum = checksum;
    
    return result;
}

// 保存测试结果为CSV格式
void save_csv_results(test_result* results, int count, const char* filename) {
    FILE* file = fopen(filename, "w");
    if (!file) {
        printf("无法创建文件 %s\n", filename);
        return;
    }
    
    // CSV头部
    fprintf(file, "算法名称,总耗时(ms),每次操作耗时(ns),吞吐量(Mops/sec),校验和,迭代次数\n");
    
    // 数据行
    for (int i = 0; i < count; i++) {
        fprintf(file, "%s,%.2f,%.2f,%.2f,%llu,%d\n",
                results[i].algorithm_name,
                results[i].total_time_ms,
                results[i].time_per_operation_ns,
                results[i].throughput_mops,
                results[i].checksum,
                results[i].iterations);
    }
    
    fclose(file);
    printf("✅ 测试结果已保存到 %s\n", filename);
}

// 保存测试结果为JSON格式
void save_json_results(test_result* results, int count, const char* filename) {
    FILE* file = fopen(filename, "w");
    if (!file) {
        printf("无法创建文件 %s\n", filename);
        return;
    }
    
    fprintf(file, "{\n");
    fprintf(file, "  \"test_info\": {\n");
    fprintf(file, "    \"test_time\": \"%s\",\n", "2024-10-19");
    fprintf(file, "    \"total_algorithms\": %d,\n", count);
    fprintf(file, "    \"test_data\": {\n");
    fprintf(file, "      \"TEST_A\": \"0x123456789ABCDEF0FEDCBA987654321000112233445566771899AABBCCDDEEFF\",\n");
    fprintf(file, "      \"TEST_B\": \"0xAAAAAAAAAAAAAAAA5555555555555555CCCCCCCCCCCCCCCC1333333333333333\"\n");
    fprintf(file, "    }\n");
    fprintf(file, "  },\n");
    fprintf(file, "  \"results\": [\n");
    
    for (int i = 0; i < count; i++) {
        fprintf(file, "    {\n");
        fprintf(file, "      \"algorithm_name\": \"%s\",\n", results[i].algorithm_name);
        fprintf(file, "      \"total_time_ms\": %.2f,\n", results[i].total_time_ms);
        fprintf(file, "      \"time_per_operation_ns\": %.2f,\n", results[i].time_per_operation_ns);
        fprintf(file, "      \"throughput_mops\": %.2f,\n", results[i].throughput_mops);
        fprintf(file, "      \"checksum\": %llu,\n", results[i].checksum);
        fprintf(file, "      \"iterations\": %d\n", results[i].iterations);
        fprintf(file, "    }%s\n", (i < count - 1) ? "," : "");
    }
    
    fprintf(file, "  ]\n");
    fprintf(file, "}\n");
    
    fclose(file);
    printf("✅ 测试结果已保存到 %s\n", filename);
}

// 保存测试日志
void save_test_log(test_result* results, int count, const char* filename) {
    FILE* file = fopen(filename, "w");
    if (!file) {
        printf("无法创建文件 %s\n", filename);
        return;
    }
    
    fprintf(file, "==============================================\n");
    fprintf(file, "CSIDH-256 性能测试日志\n");
    fprintf(file, "==============================================\n");
    fprintf(file, "测试时间: %s\n", "2024-10-19");
    fprintf(file, "测试环境: Windows MSYS2/MINGW64\n");
    fprintf(file, "编译器: GCC\n");
    fprintf(file, "优化级别: -O3\n");
    fprintf(file, "\n");
    
    fprintf(file, "测试数据:\n");
    fprintf(file, "TEST_A: 0x123456789ABCDEF0FEDCBA987654321000112233445566771899AABBCCDDEEFF\n");
    fprintf(file, "TEST_B: 0xAAAAAAAAAAAAAAAA5555555555555555CCCCCCCCCCCCCCCC1333333333333333\n");
    fprintf(file, "\n");
    
    fprintf(file, "测试结果:\n");
    fprintf(file, "==============================================\n");
    
    for (int i = 0; i < count; i++) {
        fprintf(file, "算法: %s\n", results[i].algorithm_name);
        fprintf(file, "  总耗时: %.2f ms\n", results[i].total_time_ms);
        fprintf(file, "  每次操作耗时: %.2f ns\n", results[i].time_per_operation_ns);
        fprintf(file, "  吞吐量: %.2f M ops/sec\n", results[i].throughput_mops);
        fprintf(file, "  校验和: %llu\n", results[i].checksum);
        fprintf(file, "  迭代次数: %d\n", results[i].iterations);
        fprintf(file, "\n");
    }
    
    fclose(file);
    printf("✅ 测试日志已保存到 %s\n", filename);
}

int main() {
    printf("==============================================\n");
    printf("CSIDH-256 测试数据收集程序\n");
    printf("==============================================\n\n");
    
    // 初始化Montgomery域
    mont_field mf_optimized;
    mont_field_init_optimized(&mf_optimized);
    
    const int iterations = 100000;
    test_result results[2];
    
    printf("开始性能测试...\n");
    printf("迭代次数: %d\n\n", iterations);
    
    // 测试传统算法
    printf("测试传统模乘算法...\n");
    results[0] = run_traditional_test("传统模乘算法", traditional_mod_mul_real, iterations);
    printf("✅ 完成！耗时: %.2f ms, 每次操作: %.2f ns\n", 
           results[0].total_time_ms, results[0].time_per_operation_ns);
    
    // 测试优化Montgomery算法
    printf("\n测试优化Montgomery算法...\n");
    results[1] = run_performance_test("优化Montgomery算法", mont_mul_optimized, &mf_optimized, iterations);
    printf("✅ 完成！耗时: %.2f ms, 每次操作: %.2f ns\n", 
           results[1].total_time_ms, results[1].time_per_operation_ns);
    
    // 计算性能提升
    double speedup = results[0].time_per_operation_ns / results[1].time_per_operation_ns;
    double improvement = (speedup - 1.0) * 100.0;
    
    printf("\n==============================================\n");
    printf("性能对比结果\n");
    printf("==============================================\n");
    printf("传统算法耗时: %.2f ns/op\n", results[0].time_per_operation_ns);
    printf("优化算法耗时: %.2f ns/op\n", results[1].time_per_operation_ns);
    printf("性能提升: %.2fx (%.1f%%)\n", speedup, improvement);
    printf("==============================================\n\n");
    
    // 保存测试数据
    printf("保存测试数据...\n");
    save_csv_results(results, 2, "test_results.csv");
    save_json_results(results, 2, "performance_data.json");
    save_test_log(results, 2, "benchmark_log.txt");
    
    printf("\n✅ 所有测试数据已保存完成！\n");
    printf("生成的文件:\n");
    printf("- test_results.csv (CSV格式)\n");
    printf("- performance_data.json (JSON格式)\n");
    printf("- benchmark_log.txt (文本日志)\n");
    
    return 0;
}



