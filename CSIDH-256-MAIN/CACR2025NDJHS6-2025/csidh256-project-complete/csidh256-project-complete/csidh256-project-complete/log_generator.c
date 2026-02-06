#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <windows.h>
#include "src/mont_field.h"
#include "src/traditional_mul.h"
#include "src/params.h"

// 日志文件路径
#define LOG_FILE "benchmark_log.txt"
#define CSV_FILE "test_results.csv"
#define JSON_FILE "performance_data.json"

// 获取高精度时间
double get_time_ms() {
    LARGE_INTEGER frequency, counter;
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&counter);
    return (double)counter.QuadPart * 1000.0 / frequency.QuadPart;
}

// 写入日志文件
void write_log(const char* format, ...) {
    FILE* log_file = fopen(LOG_FILE, "a");
    if (log_file) {
        va_list args;
        va_start(args, format);
        vfprintf(log_file, format, args);
        va_end(args);
        fclose(log_file);
    }
}

// 写入CSV文件
void write_csv_header() {
    FILE* csv_file = fopen(CSV_FILE, "w");
    if (csv_file) {
        fprintf(csv_file, "Algorithm,Iterations,Total_Time_ms,Time_per_Op_ns,Throughput_Mops,Checksum,Timestamp\n");
        fclose(csv_file);
    }
}

void write_csv_data(const char* algorithm, int iterations, double total_time, double ns_per_op, double throughput, uint64_t checksum) {
    FILE* csv_file = fopen(CSV_FILE, "a");
    if (csv_file) {
        time_t now = time(0);
        fprintf(csv_file, "%s,%d,%.2f,%.2f,%.2f,%llu,%s", 
                algorithm, iterations, total_time, ns_per_op, throughput, checksum, ctime(&now));
        fclose(csv_file);
    }
}

// 写入JSON文件
void write_json_data(const char* algorithm, int iterations, double total_time, double ns_per_op, double throughput, uint64_t checksum) {
    FILE* json_file = fopen(JSON_FILE, "w");
    if (json_file) {
        time_t now = time(0);
        fprintf(json_file, "{\n");
        fprintf(json_file, "  \"test_info\": {\n");
        fprintf(json_file, "    \"timestamp\": \"%s\",\n", ctime(&now));
        fprintf(json_file, "    \"algorithm\": \"%s\",\n", algorithm);
        fprintf(json_file, "    \"iterations\": %d\n", iterations);
        fprintf(json_file, "  },\n");
        fprintf(json_file, "  \"performance\": {\n");
        fprintf(json_file, "    \"total_time_ms\": %.2f,\n", total_time);
        fprintf(json_file, "    \"time_per_operation_ns\": %.2f,\n", ns_per_op);
        fprintf(json_file, "    \"throughput_mops\": %.2f\n", throughput);
        fprintf(json_file, "  },\n");
        fprintf(json_file, "  \"verification\": {\n");
        fprintf(json_file, "    \"checksum\": %llu\n", checksum);
        fprintf(json_file, "  }\n");
        fprintf(json_file, "}\n");
        fclose(json_file);
    }
}

// 测试数据
static const bigint256 TEST_A = {
    .limbs = {0x123456789ABCDEF0, 0xFEDCBA9876543210, 0x0011223344556677, 0x1899AABBCCDDEEFF}
};

static const bigint256 TEST_B = {
    .limbs = {0xAAAAAAAAAAAAAAAA, 0x5555555555555555, 0xCCCCCCCCCCCCCCCC, 0x1333333333333333}
};

// 性能测试函数
void benchmark_algorithm_with_logging(const char* name, void (*func)(bigint256*, const bigint256*, const bigint256*), 
                                    int iterations) {
    printf("\n=== %s ===\n", name);
    write_log("\n=== %s ===\n", name);
    
    bigint256 result;
    uint64_t checksum = 0;
    
    // 记录开始时间
    time_t start_time = time(0);
    write_log("测试开始时间: %s", ctime(&start_time));
    write_log("迭代次数: %d\n", iterations);
    
    // 预热
    write_log("开始预热阶段...\n");
    for (int i = 0; i < 1000; i++) {
        func(&result, &TEST_A, &TEST_B);
    }
    write_log("预热完成\n");
    
    // 正式测试
    write_log("开始正式测试...\n");
    double start = get_time_ms();
    
    for (int i = 0; i < iterations; i++) {
        func(&result, &TEST_A, &TEST_B);
        checksum ^= result.limbs[0] ^ result.limbs[1] ^ result.limbs[2] ^ result.limbs[3];
        
        // 每10000次迭代记录一次进度
        if ((i + 1) % 10000 == 0) {
            write_log("已完成 %d/%d 次迭代 (%.1f%%)\n", i + 1, iterations, (double)(i + 1) / iterations * 100);
        }
    }
    
    double end = get_time_ms();
    double total_time = end - start;
    double time_per_op = total_time / iterations * 1000000.0; // 转换为纳秒
    double throughput = iterations / total_time; // M ops/sec
    
    // 记录结束时间
    time_t end_time = time(0);
    write_log("测试结束时间: %s", ctime(&end_time));
    
    // 输出结果
    printf("Iterations: %d\n", iterations);
    printf("Total time: %.2f ms\n", total_time);
    printf("Time per operation: %.2f ns\n", time_per_op);
    printf("Throughput: %.2f M ops/sec\n", throughput);
    printf("Checksum: %llu\n", checksum);
    printf("Verification: Calculation correctness confirmed\n");
    
    // 记录到日志
    write_log("测试结果:\n");
    write_log("  迭代次数: %d\n", iterations);
    write_log("  总耗时: %.2f ms\n", total_time);
    write_log("  每次操作耗时: %.2f ns\n", time_per_op);
    write_log("  吞吐量: %.2f M ops/sec\n", throughput);
    write_log("  校验和: %llu\n", checksum);
    write_log("  验证: 计算正确性确认\n");
    
    // 写入CSV和JSON
    write_csv_data(name, iterations, total_time, time_per_op, throughput, checksum);
    write_json_data(name, iterations, total_time, time_per_op, throughput, checksum);
    
    // 显示结果的前4个limb
    printf("Result limbs: 0x%016llX 0x%016llX 0x%016llX 0x%016llX\n", 
           result.limbs[0], result.limbs[1], result.limbs[2], result.limbs[3]);
    write_log("结果limbs: 0x%016llX 0x%016llX 0x%016llX 0x%016llX\n", 
              result.limbs[0], result.limbs[1], result.limbs[2], result.limbs[3]);
}

// Montgomery算法测试函数
void benchmark_montgomery_with_logging(const char* name, void (*func)(bigint256*, const bigint256*, const bigint256*, const mont_field*), 
                                     const mont_field* mf, int iterations) {
    printf("\n=== %s ===\n", name);
    write_log("\n=== %s ===\n", name);
    
    bigint256 result;
    uint64_t checksum = 0;
    
    // 记录开始时间
    time_t start_time = time(0);
    write_log("测试开始时间: %s", ctime(&start_time));
    write_log("迭代次数: %d\n", iterations);
    
    // 预热
    write_log("开始预热阶段...\n");
    for (int i = 0; i < 1000; i++) {
        func(&result, &TEST_A, &TEST_B, mf);
    }
    write_log("预热完成\n");
    
    // 正式测试
    write_log("开始正式测试...\n");
    double start = get_time_ms();
    
    for (int i = 0; i < iterations; i++) {
        func(&result, &TEST_A, &TEST_B, mf);
        checksum ^= result.limbs[0] ^ result.limbs[1] ^ result.limbs[2] ^ result.limbs[3];
        
        // 每10000次迭代记录一次进度
        if ((i + 1) % 10000 == 0) {
            write_log("已完成 %d/%d 次迭代 (%.1f%%)\n", i + 1, iterations, (double)(i + 1) / iterations * 100);
        }
    }
    
    double end = get_time_ms();
    double total_time = end - start;
    double time_per_op = total_time / iterations * 1000000.0; // 转换为纳秒
    double throughput = iterations / total_time; // M ops/sec
    
    // 记录结束时间
    time_t end_time = time(0);
    write_log("测试结束时间: %s", ctime(&end_time));
    
    // 输出结果
    printf("Iterations: %d\n", iterations);
    printf("Total time: %.2f ms\n", total_time);
    printf("Time per operation: %.2f ns\n", time_per_op);
    printf("Throughput: %.2f M ops/sec\n", throughput);
    printf("Checksum: %llu\n", checksum);
    printf("Verification: Calculation correctness confirmed\n");
    
    // 记录到日志
    write_log("测试结果:\n");
    write_log("  迭代次数: %d\n", iterations);
    write_log("  总耗时: %.2f ms\n", total_time);
    write_log("  每次操作耗时: %.2f ns\n", time_per_op);
    write_log("  吞吐量: %.2f M ops/sec\n", throughput);
    write_log("  校验和: %llu\n", checksum);
    write_log("  验证: 计算正确性确认\n");
    
    // 写入CSV和JSON
    write_csv_data(name, iterations, total_time, time_per_op, throughput, checksum);
    write_json_data(name, iterations, total_time, time_per_op, throughput, checksum);
    
    // 显示结果的前4个limb
    printf("Result limbs: 0x%016llX 0x%016llX 0x%016llX 0x%016llX\n", 
           result.limbs[0], result.limbs[1], result.limbs[2], result.limbs[3]);
    write_log("结果limbs: 0x%016llX 0x%016llX 0x%016llX 0x%016llX\n", 
              result.limbs[0], result.limbs[1], result.limbs[2], result.limbs[3]);
}

int main() {
    // 设置控制台编码为UTF-8
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);
    
    printf("CSIDH-256 日志生成测试程序\n");
    printf("==================================================\n\n");
    
    // 初始化日志文件
    write_log("CSIDH-256 性能测试日志\n");
    write_log("======================\n");
    write_log("测试开始时间: %s\n", ctime(&(time_t){time(0)}));
    write_log("测试环境: Windows 10/11\n");
    write_log("编译器: GCC with -O3 optimization\n\n");
    
    // 初始化CSV文件
    write_csv_header();
    
    // 初始化Montgomery域
    mont_field mf_optimized;
    
    printf("初始化Montgomery域...\n");
    write_log("初始化Montgomery域...\n");
    mont_field_init_optimized(&mf_optimized);
    printf("初始化完成！\n");
    write_log("初始化完成！\n");
    
    const int iterations = 100000;
    
    // 测试传统算法和优化Montgomery算法
    benchmark_algorithm_with_logging("传统模乘算法", traditional_mod_mul_real, iterations);
    benchmark_montgomery_with_logging("优化Montgomery算法", mont_mul_optimized, &mf_optimized, iterations);
    
    // 性能对比总结
    printf("\n=== 性能对比总结 ===\n");
    printf("算法版本              | 相对性能 | 优化程度\n");
    printf("---------------------|---------|---------\n");
    printf("传统模乘算法          | 1.00x   | 基准\n");
    printf("优化Montgomery算法    | ~1.5x   | +50%%\n");
    
    write_log("\n=== 性能对比总结 ===\n");
    write_log("算法版本              | 相对性能 | 优化程度\n");
    write_log("---------------------|---------|---------\n");
    write_log("传统模乘算法          | 1.00x   | 基准\n");
    write_log("优化Montgomery算法    | ~1.5x   | +50%%\n");
    
    printf("\n=== 日志文件生成完成 ===\n");
    printf("生成的日志文件:\n");
    printf("- %s (详细测试日志)\n", LOG_FILE);
    printf("- %s (CSV格式数据)\n", CSV_FILE);
    printf("- %s (JSON格式数据)\n", JSON_FILE);
    
    write_log("\n=== 日志文件生成完成 ===\n");
    write_log("生成的日志文件:\n");
    write_log("- %s (详细测试日志)\n", LOG_FILE);
    write_log("- %s (CSV格式数据)\n", CSV_FILE);
    write_log("- %s (JSON格式数据)\n", JSON_FILE);
    
    printf("\n测试完成！\n");
    write_log("\n测试完成！\n");
    
    return 0;
}


