#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>

// 256位大整数结构
typedef struct {
    uint64_t limbs[4];
} bigint256;

// 性能测试结果结构
typedef struct {
    char algorithm_name[64];
    double total_time_ms;
    double time_per_operation_ns;
    double throughput_mops;
    uint64_t checksum;
    int iterations;
    double cache_miss_rate;
    double memory_usage_mb;
} performance_result;

// 模拟传统算法
uint64_t traditional_multiply(bigint256* a, bigint256* b) {
    // 简化的传统乘法实现
    uint64_t result = 0;
    for (int i = 0; i < 4; i++) {
        result += a->limbs[i] * b->limbs[i];
    }
    return result;
}

// 模拟Montgomery算法
uint64_t montgomery_multiply(bigint256* a, bigint256* b) {
    // 简化的Montgomery乘法实现
    uint64_t result = 0;
    for (int i = 0; i < 4; i++) {
        result += (a->limbs[i] * b->limbs[i]) % 0xFFFFFFFFFFFFFFF1;
    }
    return result;
}

// 模拟优化Montgomery算法
uint64_t optimized_montgomery_multiply(bigint256* a, bigint256* b) {
    // 简化的优化Montgomery乘法实现
    uint64_t result = 0;
    for (int i = 0; i < 4; i++) {
        uint64_t temp = a->limbs[i] * b->limbs[i];
        result += temp - (temp / 0xFFFFFFFFFFFFFFF1) * 0xFFFFFFFFFFFFFFF1;
    }
    return result;
}

// 获取高精度时间
double get_time_ms() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1000.0 + ts.tv_nsec / 1000000.0;
}

// 运行性能测试
void run_performance_test(const char* algorithm_name, 
                         uint64_t (*multiply_func)(bigint256*, bigint256*),
                         bigint256* test_a, bigint256* test_b,
                         int iterations, performance_result* result) {
    
    printf("🧪 测试算法: %s\n", algorithm_name);
    printf("   迭代次数: %d\n", iterations);
    
    // 预热
    for (int i = 0; i < 1000; i++) {
        multiply_func(test_a, test_b);
    }
    
    // 开始计时
    double start_time = get_time_ms();
    uint64_t checksum = 0;
    
    for (int i = 0; i < iterations; i++) {
        checksum += multiply_func(test_a, test_b);
    }
    
    double end_time = get_time_ms();
    double total_time = end_time - start_time;
    
    // 计算结果
    result->total_time_ms = total_time;
    result->time_per_operation_ns = (total_time * 1000000.0) / iterations;
    result->throughput_mops = iterations / (total_time / 1000.0) / 1000000.0;
    result->checksum = checksum;
    result->iterations = iterations;
    result->cache_miss_rate = 0.05; // 模拟缓存未命中率
    result->memory_usage_mb = 0.1;  // 模拟内存使用量
    
    printf("   总时间: %.2f ms\n", result->total_time_ms);
    printf("   每次操作: %.2f ns\n", result->time_per_operation_ns);
    printf("   吞吐量: %.2f M ops/sec\n", result->throughput_mops);
    printf("   校验和: 0x%016llx\n", result->checksum);
    printf("\n");
}

// 分析性能差异
void analyze_performance_difference(performance_result* traditional, 
                                   performance_result* montgomery,
                                   performance_result* optimized) {
    
    printf("📊 性能分析报告\n");
    printf("==============================================\n\n");
    
    printf("🔍 算法性能对比:\n");
    printf("----------------------------------------------\n");
    printf("传统算法:           %.2f ns/op, %.2f M ops/sec\n", 
           traditional->time_per_operation_ns, traditional->throughput_mops);
    printf("Montgomery算法:     %.2f ns/op, %.2f M ops/sec\n", 
           montgomery->time_per_operation_ns, montgomery->throughput_mops);
    printf("优化Montgomery算法: %.2f ns/op, %.2f M ops/sec\n", 
           optimized->time_per_operation_ns, optimized->throughput_mops);
    
    printf("\n📈 性能提升分析:\n");
    printf("----------------------------------------------\n");
    
    // 计算相对于传统算法的提升
    double montgomery_speedup = traditional->time_per_operation_ns / montgomery->time_per_operation_ns;
    double optimized_speedup = traditional->time_per_operation_ns / optimized->time_per_operation_ns;
    
    printf("Montgomery vs 传统:     %.2fx %s\n", 
           montgomery_speedup, montgomery_speedup > 1.0 ? "(更快)" : "(更慢)");
    printf("优化Montgomery vs 传统: %.2fx %s\n", 
           optimized_speedup, optimized_speedup > 1.0 ? "(更快)" : "(更慢)");
    
    printf("\n🔍 可能的原因分析:\n");
    printf("----------------------------------------------\n");
    
    if (montgomery_speedup < 1.0) {
        printf("❌ Montgomery算法比传统算法慢的原因:\n");
        printf("   1. 转换开销: Montgomery算法需要额外的转换步骤\n");
        printf("   2. 小数据规模: 对于小规模数据，转换开销可能超过优化收益\n");
        printf("   3. 实现效率: 当前实现可能还有优化空间\n");
        printf("   4. 内存访问: 内存访问模式可能不够优化\n");
    }
    
    if (optimized_speedup < 1.0) {
        printf("❌ 优化Montgomery算法比传统算法慢的原因:\n");
        printf("   1. 优化开销: 优化步骤可能增加了计算复杂度\n");
        printf("   2. 常数选择: 优化常数可能不够理想\n");
        printf("   3. 分支预测: 优化可能影响了分支预测效率\n");
        printf("   4. 缓存效率: 优化可能影响了缓存使用效率\n");
    }
    
    printf("\n💡 改进建议:\n");
    printf("----------------------------------------------\n");
    printf("1. 增加测试数据规模，测试大数据场景下的性能\n");
    printf("2. 优化Montgomery算法的实现，减少转换开销\n");
    printf("3. 改进内存访问模式，提高缓存效率\n");
    printf("4. 调整优化参数，找到最优的常数选择\n");
    printf("5. 使用更高效的编译器优化选项\n");
}

// 测试不同数据规模
void test_different_data_sizes() {
    printf("📊 不同数据规模性能测试\n");
    printf("==============================================\n\n");
    
    // 测试数据
    bigint256 small_a = {{1, 0, 0, 0}};
    bigint256 small_b = {{2, 0, 0, 0}};
    
    bigint256 medium_a = {{0x123456789ABCDEF0, 0xFEDCBA9876543210, 0, 0}};
    bigint256 medium_b = {{0xAAAAAAAAAAAAAAAA, 0x5555555555555555, 0, 0}};
    
    bigint256 large_a = {{0x123456789ABCDEF0, 0xFEDCBA9876543210, 0x0011223344556677, 0x1899AABBCCDDEEFF}};
    bigint256 large_b = {{0xAAAAAAAAAAAAAAAA, 0x5555555555555555, 0xCCCCCCCCCCCCCCCC, 0x1333333333333333}};
    
    performance_result results[3];
    
    printf("🔍 小数据测试 (64位):\n");
    printf("----------------------------------------------\n");
    run_performance_test("传统算法", traditional_multiply, &small_a, &small_b, 1000000, &results[0]);
    run_performance_test("Montgomery算法", montgomery_multiply, &small_a, &small_b, 1000000, &results[1]);
    run_performance_test("优化Montgomery算法", optimized_montgomery_multiply, &small_a, &small_b, 1000000, &results[2]);
    
    printf("🔍 中等数据测试 (128位):\n");
    printf("----------------------------------------------\n");
    run_performance_test("传统算法", traditional_multiply, &medium_a, &medium_b, 1000000, &results[0]);
    run_performance_test("Montgomery算法", montgomery_multiply, &medium_a, &medium_b, 1000000, &results[1]);
    run_performance_test("优化Montgomery算法", optimized_montgomery_multiply, &medium_a, &medium_b, 1000000, &results[2]);
    
    printf("🔍 大数据测试 (256位):\n");
    printf("----------------------------------------------\n");
    run_performance_test("传统算法", traditional_multiply, &large_a, &large_b, 1000000, &results[0]);
    run_performance_test("Montgomery算法", montgomery_multiply, &large_a, &large_b, 1000000, &results[1]);
    run_performance_test("优化Montgomery算法", optimized_montgomery_multiply, &large_a, &large_b, 1000000, &results[2]);
    
    analyze_performance_difference(&results[0], &results[1], &results[2]);
}

int main() {
    printf("==============================================\n");
    printf("CSIDH-256 详细性能分析工具\n");
    printf("==============================================\n\n");
    
    printf("🎯 分析目标: 为什么传统算法比优化算法耗时更少？\n\n");
    
    // 运行不同数据规模的测试
    test_different_data_sizes();
    
    printf("\n==============================================\n");
    printf("性能分析完成\n");
    printf("==============================================\n");
    
    return 0;
}

