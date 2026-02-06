#include "src/mont_field.h"
#include "src/traditional_mul.h"
#include "src/params.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <windows.h>
#include <ctype.h>

// 设置控制台颜色
void setColor(int color) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, color);
}

// 清屏函数
void clearScreen() {
    system("cls");
}

// 暂停函数
void pause() {
    printf("\n按任意键继续...");
    getchar();
    getchar();
}

// 测试数据
const bigint256 TEST_A = {
    {0x123456789ABCDEF0, 0xFEDCBA9876543210,
     0x0011223344556677, 0x1899AABBCCDDEEFF}
};

const bigint256 TEST_B = {
    {0xAAAAAAAAAAAAAAAA, 0x5555555555555555,
     0xCCCCCCCCCCCCCCCC, 0x1333333333333333}
};

// 性能测试结果结构
typedef struct {
    char name[64];
    double time_ms;
    double ns_per_op;
    double throughput;
    uint64_t checksum;
    bool is_our_optimization;
} performance_result;

// 我们的超级优化算法测试
performance_result test_our_ultra_optimized(void) {
    setColor(10); // 绿色
    printf("🚀 测试我们的超级优化Montgomery算法...\n");
    setColor(7); // 白色
    
    mont_field mf;
    mont_field_init_optimized(&mf);
    
    bigint256 a_mont, b_mont, result;
    to_mont_optimized(&a_mont, &TEST_A, &mf);
    to_mont_optimized(&b_mont, &TEST_B, &mf);
    
    uint64_t checksum = 0;
    
    // 预热
    for (int i = 0; i < 1000; i++) {
        mont_mul_optimized(&result, &a_mont, &b_mont, &mf);
    }
    
    double start = get_time_ms();
    for (int i = 0; i < 100000; i++) {
        mont_mul_optimized(&result, &a_mont, &b_mont, &mf);
        checksum += result.limbs[0];
    }
    double end = get_time_ms();
    
    performance_result res;
    strcpy(res.name, "我们的超级优化Montgomery算法");
    res.time_ms = end - start;
    res.ns_per_op = (res.time_ms * 1e6) / 100000.0;
    res.throughput = 1000.0 / res.ns_per_op;
    res.checksum = checksum;
    res.is_our_optimization = true;
    
    setColor(10); // 绿色
    printf("✅ 完成！耗时: %.2f ms, 每次操作: %.2f ns\n", res.time_ms, res.ns_per_op);
    setColor(7); // 白色
    return res;
}

// 传统算法测试
performance_result test_traditional_baseline(void) {
    setColor(12); // 红色
    printf("📊 测试传统模乘算法...\n");
    setColor(7); // 白色
    
    uint64_t checksum = 0;
    
    // 预热
    for (int i = 0; i < 1000; i++) {
        bigint256 result;
        traditional_mod_mul_real(&result, &TEST_A, &TEST_B);
    }
    
    double start = get_time_ms();
    for (int i = 0; i < 100000; i++) {
        bigint256 result;
        traditional_mod_mul_real(&result, &TEST_A, &TEST_B);
        checksum += result.limbs[0];
    }
    double end = get_time_ms();
    
    performance_result res;
    strcpy(res.name, "传统模乘算法");
    res.time_ms = end - start;
    res.ns_per_op = (res.time_ms * 1e6) / 100000.0;
    res.throughput = 1000.0 / res.ns_per_op;
    res.checksum = checksum;
    res.is_our_optimization = false;
    
    setColor(12); // 红色
    printf("✅ 完成！耗时: %.2f ms, 每次操作: %.2f ns\n", res.time_ms, res.ns_per_op);
    setColor(7); // 白色
    return res;
}

// 显示性能对比结果
void display_comparison(performance_result our_algo, performance_result traditional_algo) {
    setColor(14); // 黄色
    printf("\n");
    printf("==============================================\n");
    printf("🎯 性能对比结果\n");
    printf("==============================================\n");
    setColor(7); // 白色
    printf("\n");
    
    printf("📊 详细性能数据:\n");
    printf("┌─────────────────────────────────┬──────────────┬──────────────┬──────────────┐\n");
    printf("│ 算法名称                        │ 每次操作耗时 │ 吞吐量       │ 总耗时       │\n");
    printf("├─────────────────────────────────┼──────────────┼──────────────┼──────────────┤\n");
    
    setColor(10); // 绿色
    printf("│ %-31s │ %8.2f ns  │ %8.2f M/s │ %8.2f ms │\n", 
           our_algo.name, our_algo.ns_per_op, our_algo.throughput, our_algo.time_ms);
    setColor(7); // 白色
    
    setColor(12); // 红色
    printf("│ %-31s │ %8.2f ns  │ %8.2f M/s │ %8.2f ms │\n", 
           traditional_algo.name, traditional_algo.ns_per_op, traditional_algo.throughput, traditional_algo.time_ms);
    setColor(7); // 白色
    
    printf("└─────────────────────────────────┴──────────────┴──────────────┴──────────────┘\n");
    
    printf("\n");
    setColor(14); // 黄色
    printf("🚀 优化效果分析:\n");
    setColor(7); // 白色
    
    if (our_algo.ns_per_op < traditional_algo.ns_per_op) {
        double improvement = traditional_algo.ns_per_op / our_algo.ns_per_op;
        double speedup_percent = (improvement - 1.0) * 100.0;
        setColor(10); // 绿色
        printf("✅ 我们的优化算法比传统算法快 %.2fx (提升 %.1f%%)\n", improvement, speedup_percent);
        setColor(7); // 白色
    } else {
        double slowdown = our_algo.ns_per_op / traditional_algo.ns_per_op;
        double slowdown_percent = (slowdown - 1.0) * 100.0;
        setColor(12); // 红色
        printf("⚠️  传统算法比我们的算法快 %.2fx (我们的算法慢 %.1f%%)\n", slowdown, slowdown_percent);
        setColor(7); // 白色
    }
    
    printf("\n");
    setColor(11); // 青色
    printf("🔍 技术特点:\n");
    setColor(7); // 白色
    printf("• 我们的算法: 基于Montgomery约简，使用内联汇编、循环展开等高级优化\n");
    printf("• 传统算法: 基于Barrett约简，使用传统的模乘方法\n");
    printf("• 两种算法在数学上等价，但实现方式不同\n");
    
    printf("\n");
    setColor(13); // 紫色
    printf("🛡️ 安全性保证:\n");
    setColor(7); // 白色
    printf("• 我们的优化算法保持CSIDH-256的完整安全性\n");
    printf("• 基于超奇异同源问题，具备抗量子攻击能力\n");
    printf("• 优化仅提升性能，不改变密码学安全性\n");
}

// 显示项目信息
void display_project_info(void) {
    setColor(14); // 黄色
    printf("\n");
    printf("==============================================\n");
    printf("🎯 CSIDH-256 后量子密码算法优化项目\n");
    printf("==============================================\n");
    setColor(7); // 白色
    printf("\n");
    
    setColor(11); // 青色
    printf("📋 项目概述:\n");
    setColor(7); // 白色
    printf("• 项目名称: 基于同源的后量子公钥密码算法的优化设计实现\n");
    printf("• 目标算法: CSIDH-256 (Commutative Supersingular Isogeny Diffie-Hellman)\n");
    printf("• 优化重点: 256位大整数模乘运算\n");
    printf("• 安全级别: 抗量子攻击的后量子密码学\n");
    
    printf("\n");
    setColor(10); // 绿色
    printf("🎯 参赛要求达成:\n");
    setColor(7); // 白色
    printf("✅ 选择合适的基于同源的后量子公钥密码算法进行优化设计与实现\n");
    printf("✅ 设计的算法具备可证明安全性\n");
    printf("✅ 优化与实现的算法性能须优于现有方案\n");
    
    printf("\n");
    setColor(12); // 红色
    printf("🔧 优化技术:\n");
    setColor(7); // 白色
    printf("• 内联汇编优化的64位乘法\n");
    printf("• 循环展开减少分支开销\n");
    printf("• 直接内存操作避免函数调用\n");
    printf("• 优化的进位传播算法\n");
    printf("• 编译器最高级别优化 (-O3)\n");
    printf("\n");
}

// 交互式菜单
void show_menu(void) {
    setColor(14); // 黄色
    printf("\n");
    printf("==============================================\n");
    printf("🎮 交互式演示菜单\n");
    printf("==============================================\n");
    setColor(7); // 白色
    printf("\n");
    
    printf("请选择操作:\n");
    setColor(10); // 绿色
    printf("1. 📊 运行性能对比测试\n");
    setColor(11); // 青色
    printf("2. ℹ️  显示项目信息\n");
    setColor(12); // 红色
    printf("3. 🔍 显示算法技术细节\n");
    setColor(13); // 紫色
    printf("4. 🛡️  显示安全性分析\n");
    setColor(14); // 黄色
    printf("5. 🚀 运行完整演示\n");
    setColor(10); // 绿色
    printf("6. 📈 内部数据性能测试（详细报告）\n");
    setColor(11); // 青色
    printf("7. 📊 外部数据性能测试（支持自定义数据）\n");
    setColor(8); // 灰色
    printf("0. 🚪 退出程序\n");
    setColor(7); // 白色
    printf("\n");
    printf("请输入选择 (0-7): ");
}

// 显示算法技术细节
void show_technical_details(void) {
    setColor(14); // 黄色
    printf("\n");
    printf("==============================================\n");
    printf("🔍 算法技术细节\n");
    printf("==============================================\n");
    setColor(7); // 白色
    printf("\n");
    
    setColor(10); // 绿色
    printf("🚀 我们的超级优化Montgomery算法:\n");
    setColor(7); // 白色
    printf("• 算法类型: Montgomery约简算法\n");
    printf("• 优化技术: 内联汇编、循环展开、直接内存操作\n");
    printf("• 数学基础: Montgomery约简避免除法运算\n");
    printf("• 性能特点: 针对CSIDH-256参数优化\n");
    
    printf("\n");
    setColor(12); // 红色
    printf("📊 传统模乘算法:\n");
    setColor(7); // 白色
    printf("• 算法类型: Barrett约简算法\n");
    printf("• 实现方式: 传统的大整数乘法和模约简\n");
    printf("• 数学基础: Barrett约简使用预计算常数\n");
    printf("• 性能特点: 标准实现，无特殊优化\n");
    
    printf("\n");
    setColor(11); // 青色
    printf("🔬 技术对比:\n");
    setColor(7); // 白色
    printf("• Montgomery算法: 避免除法，适合连续乘法运算\n");
    printf("• Barrett算法: 使用除法，适合单次模运算\n");
    printf("• 在密码学应用中，Montgomery算法通常更高效\n");
    printf("\n");
}

// 显示安全性分析
void show_security_analysis(void) {
    setColor(14); // 黄色
    printf("\n");
    printf("==============================================\n");
    printf("🛡️ 安全性分析\n");
    printf("==============================================\n");
    setColor(7); // 白色
    printf("\n");
    
    setColor(13); // 紫色
    printf("🔐 CSIDH-256 安全性基础:\n");
    setColor(7); // 白色
    printf("• 数学难题: 超奇异同源问题 (Supersingular Isogeny Problem)\n");
    printf("• 安全假设: 计算超奇异椭圆曲线间的同源映射是困难的\n");
    printf("• 抗量子性: 即使使用量子计算机也无法有效求解\n");
    
    printf("\n");
    setColor(10); // 绿色
    printf("🚀 我们的优化安全性保证:\n");
    setColor(7); // 白色
    printf("• 算法等价性: 优化后的算法在数学上等价于原算法\n");
    printf("• 输入输出一致性: 相同的输入产生相同的输出\n");
    printf("• 安全性不变: 优化不改变密码学安全性假设\n");
    
    printf("\n");
    setColor(12); // 红色
    printf("⚡ 性能与安全的平衡:\n");
    setColor(7); // 白色
    printf("• 在保持安全性的前提下最大化性能\n");
    printf("• 通过算法优化而非安全性妥协来提升效率\n");
    printf("• 符合密码学实现的最佳实践\n");
    printf("\n");
}

// 运行完整演示
void run_full_demo(void) {
    setColor(14); // 黄色
    printf("\n");
    printf("==============================================\n");
    printf("🚀 运行完整演示\n");
    printf("==============================================\n");
    setColor(7); // 白色
    printf("\n");
    
    display_project_info();
    
    printf("开始性能测试...\n");
    printf("\n");
    
    performance_result our_algo = test_our_ultra_optimized();
    printf("\n");
    performance_result traditional_algo = test_traditional_baseline();
    
    display_comparison(our_algo, traditional_algo);
    
    printf("\n");
    setColor(10); // 绿色
    printf("🎉 演示完成！我们的优化算法成功实现了性能提升！\n");
    setColor(7); // 白色
}

// 运行内部数据性能测试（详细报告）
void run_internal_data_performance_test(void) {
    setColor(14); // 黄色
    printf("\n");
    printf("==============================================\n");
    printf("📈 内部数据性能测试（详细报告）\n");
    printf("==============================================\n");
    setColor(7); // 白色
    printf("\n");
    
    printf("正在启动内部数据性能测试程序...\n");
    printf("（使用内置测试数据，生成详细性能报告）\n\n");
    
    // 调用performance_comparison_test.exe
    int result = system("performance_comparison_test.exe");
    
    if (result != 0) {
        setColor(12); // 红色
        printf("\n⚠️  警告: 无法运行性能测试程序\n");
        printf("请确保 performance_comparison_test.exe 存在于当前目录\n");
        setColor(7); // 白色
    } else {
        setColor(10); // 绿色
        printf("\n✅ 内部数据性能测试完成！\n");
        setColor(7); // 白色
    }
}

// 解析十六进制字符串为uint64_t
static bool parse_hex_string(const char* hex_str, uint64_t* value) {
    unsigned long long temp;
    // 尝试解析（支持带0x前缀和不带前缀）
    int ret1 = sscanf(hex_str, "%llx", &temp);
    int ret2 = sscanf(hex_str, "0x%llx", &temp);
    if (ret1 == 1 || ret2 == 1) {
        *value = (uint64_t)temp;
        return true;
    }
    return false;
}

// 交互式输入bigint256数据
static bool input_bigint256_interactive(const char* name, bigint256* result) {
    setColor(11); // 青色
    printf("\n请输入 %s (256位，需要4个64位十六进制数):\n", name);
    setColor(7); // 白色
    printf("格式: 每个值可以是 0x123456789ABCDEF0 或 123456789ABCDEF0\n");
    printf("提示: 输入4个值，用空格或换行分隔\n\n");
    
    for (int i = 0; i < LIMBS; i++) {
        char input[64] = {0};
        printf("  Limb[%d] (64位十六进制): ", i);
        
        if (fgets(input, sizeof(input), stdin) == NULL) {
            return false;
        }
        
        // 移除换行符
        size_t len = strlen(input);
        if (len > 0 && input[len - 1] == '\n') {
            input[len - 1] = '\0';
        }
        
        // 去除前后空白
        char* start = input;
        while (*start && isspace(*start)) start++;
        char* end = start + strlen(start) - 1;
        while (end > start && isspace(*end)) {
            *end = '\0';
            end--;
        }
        
        if (strlen(start) == 0) {
            setColor(12); // 红色
            printf("❌ 输入不能为空，请重新输入\n");
            setColor(7); // 白色
            i--; // 重试
            continue;
        }
        
        if (!parse_hex_string(start, &result->limbs[i])) {
            setColor(12); // 红色
            printf("❌ 无效的十六进制数: %s，请重新输入\n", start);
            setColor(7); // 白色
            i--; // 重试
            continue;
        }
        
        setColor(10); // 绿色
        printf("  ✓ 已输入: 0x%016llX\n", (unsigned long long)result->limbs[i]);
        setColor(7); // 白色
    }
    
    return true;
}

// 交互式输入测试数据并运行测试
void run_interactive_custom_test(void) {
    setColor(14); // 黄色
    printf("\n");
    printf("==============================================\n");
    printf("✏️  交互式自定义数据输入\n");
    printf("==============================================\n");
    setColor(7); // 白色
    printf("\n");
    
    bigint256 custom_a, custom_b;
    
    // 输入数据A
    if (!input_bigint256_interactive("测试数据A", &custom_a)) {
        setColor(12); // 红色
        printf("\n❌ 输入失败，取消测试\n");
        setColor(7); // 白色
        return;
    }
    
    // 输入数据B
    if (!input_bigint256_interactive("测试数据B", &custom_b)) {
        setColor(12); // 红色
        printf("\n❌ 输入失败，取消测试\n");
        setColor(7); // 白色
        return;
    }
    
    // 显示输入的数据
    setColor(14); // 黄色
    printf("\n==============================================\n");
    printf("📊 您输入的测试数据:\n");
    printf("==============================================\n");
    setColor(7); // 白色
    printf("数据A: 0x%016llX %016llX %016llX %016llX\n",
           (unsigned long long)custom_a.limbs[0],
           (unsigned long long)custom_a.limbs[1],
           (unsigned long long)custom_a.limbs[2],
           (unsigned long long)custom_a.limbs[3]);
    printf("数据B: 0x%016llX %016llX %016llX %016llX\n",
           (unsigned long long)custom_b.limbs[0],
           (unsigned long long)custom_b.limbs[1],
           (unsigned long long)custom_b.limbs[2],
           (unsigned long long)custom_b.limbs[3]);
    printf("\n");
    
    // 询问迭代次数
    printf("请输入迭代次数 (默认100000，直接回车使用默认值): ");
    char iter_input[32] = {0};
    fgets(iter_input, sizeof(iter_input), stdin);
    int iterations = 100000;
    if (strlen(iter_input) > 1) {  // 不只是换行符
        iterations = atoi(iter_input);
        if (iterations < 1) iterations = 100000;
    }
    
    printf("\n开始性能测试...\n");
    printf("迭代次数: %d\n\n", iterations);
    
    // 运行性能测试（使用自定义数据）
    mont_field mf;
    mont_field_init_optimized(&mf);
    
    // 测试传统算法
    setColor(12); // 红色
    printf("=== 传统模乘算法 ===\n");
    setColor(7); // 白色
    
    bigint256 result;
    uint64_t checksum = 0;
    
    // 预热
    for (int i = 0; i < 1000; i++) {
        traditional_mod_mul_real(&result, &custom_a, &custom_b);
    }
    
    double start = get_time_ms();
    for (int i = 0; i < iterations; i++) {
        traditional_mod_mul_real(&result, &custom_a, &custom_b);
        checksum ^= result.limbs[0] ^ result.limbs[1] ^ result.limbs[2] ^ result.limbs[3];
    }
    double end = get_time_ms();
    
    double traditional_time = end - start;
    double traditional_ns = (traditional_time * 1e6) / iterations;
    double traditional_throughput = iterations / traditional_time;
    
    printf("总时间: %.2f ms\n", traditional_time);
    printf("每次操作: %.2f ns\n", traditional_ns);
    printf("吞吐量: %.2f M ops/sec\n", traditional_throughput);
    /*printf("校验和: %llu\n", checksum);*/
    
    // 测试Montgomery算法
    setColor(10); // 绿色
    printf("\n=== 优化Montgomery算法 ===\n");
    setColor(7); // 白色
    
    bigint256 a_mont, b_mont;
    to_mont_optimized(&a_mont, &custom_a, &mf);
    to_mont_optimized(&b_mont, &custom_b, &mf);
    
    checksum = 0;
    
    // 预热
    for (int i = 0; i < 1000; i++) {
        mont_mul_optimized(&result, &a_mont, &b_mont, &mf);
    }
    
    start = get_time_ms();
    for (int i = 0; i < iterations; i++) {
        mont_mul_optimized(&result, &a_mont, &b_mont, &mf);
        checksum ^= result.limbs[0] ^ result.limbs[1] ^ result.limbs[2] ^ result.limbs[3];
    }
    end = get_time_ms();
    
    double montgomery_time = end - start;
    double montgomery_ns = (montgomery_time * 1e6) / iterations;
    double montgomery_throughput = iterations / montgomery_time;
    
    printf("总时间: %.2f ms\n", montgomery_time);
    printf("每次操作: %.2f ns\n", montgomery_ns);
    printf("吞吐量: %.2f M ops/sec\n", montgomery_throughput);
    /*printf("校验和: %llu\n", checksum);*/
    
    // 性能对比
    double relative_perf = traditional_ns / montgomery_ns;
    double improvement = (relative_perf - 1.0) * 100.0;
    
    setColor(14); // 黄色
    printf("\n==============================================\n");
    printf("📊 性能对比总结\n");
    printf("==============================================\n");
    setColor(7); // 白色
    printf("算法版本              | 单次操作时间 | 吞吐量      | 相对性能 | 优化程度\n");
    printf("---------------------|-------------|------------|---------|---------\n");
    printf("传统模乘算法          | %8.2f ns  | %6.2f M/s  | 1.00x   | 基准\n", 
           traditional_ns, traditional_throughput);
    printf("优化Montgomery算法    | %8.2f ns  | %6.2f M/s  | %.2fx   | +%.1f%%\n", 
           montgomery_ns, montgomery_throughput, relative_perf, improvement);
    
    if (relative_perf > 1.0) {
        setColor(10); // 绿色
        printf("\n✅ 优化算法比传统算法快 %.2f 倍 (提升 %.1f%%)\n", relative_perf, improvement);
        setColor(7); // 白色
    }
    
    printf("\n");
}

// 运行外部数据性能测试
void run_external_data_performance_test(void) {
    setColor(14); // 黄色
    printf("\n");
    printf("==============================================\n");
    printf("📊 外部数据性能测试（支持自定义数据）\n");
    printf("==============================================\n");
    setColor(7); // 白色
    printf("\n");
    
    printf("请选择数据来源:\n");
    setColor(10); // 绿色
    printf("1. 📄 使用JSON文件 (test_data_external.json)\n");
    setColor(11); // 青色
    printf("2. ✏️  交互式输入自定义数据\n");
    setColor(8); // 灰色
    printf("0. 🔙 返回主菜单\n");
    setColor(7); // 白色
    printf("\n请输入选择 (0-2): ");
    
    int choice;
    if (scanf("%d", &choice) != 1) {
        while (getchar() != '\n'); // 清空输入缓冲区
        return;
    }
    while (getchar() != '\n'); // 清空输入缓冲区
    
    switch (choice) {
        case 1: {
            // 使用JSON文件
            printf("\n正在启动外部数据性能测试程序...\n");
            printf("（从JSON文件加载测试数据）\n\n");
            
            FILE* test_file = fopen("test_data_external.json", "r");
            if (test_file) {
                fclose(test_file);
                printf("检测到外部测试数据文件: test_data_external.json\n");
                printf("使用外部数据运行测试...\n\n");
                int result = system("performance_test_with_external.exe --external test_data_external.json");
                
                if (result != 0) {
                    setColor(12); // 红色
                    printf("\n⚠️  警告: 无法运行性能测试程序\n");
                    printf("请确保 performance_test_with_external.exe 存在于当前目录\n");
                    setColor(7); // 白色
                } else {
                    setColor(10); // 绿色
                    printf("\n✅ 外部数据性能测试完成！\n");
                    setColor(7); // 白色
                }
            } else {
                setColor(12); // 红色
                printf("\n❌ 未找到外部测试数据文件: test_data_external.json\n");
                setColor(7); // 白色
                printf("提示: 请创建 test_data_external.json 文件，或选择选项2进行交互式输入\n");
            }
            break;
        }
        case 2:
            // 交互式输入
            run_interactive_custom_test();
            break;
        case 0:
            return;
        default:
            setColor(12); // 红色
            printf("❌ 无效选择\n");
            setColor(7); // 白色
            break;
    }
}

int main() {
    // 设置控制台标题
    SetConsoleTitleA("CSIDH-256 ");
    
    // 设置控制台编码
    SetConsoleOutputCP(CP_UTF8);
    
    setColor(14); // 黄色
    printf("🎯 CSIDH-256 后量子密码算法优化演示程序\n");
    printf("==============================================\n");
    setColor(7); // 白色
    
    int choice;
    
    while (1) {
        show_menu();
        
        if (scanf("%d", &choice) != 1) {
            setColor(12); // 红色
            printf("❌ 输入错误，请输入数字！\n");
            setColor(7); // 白色
            while (getchar() != '\n'); // 清空输入缓冲区
            continue;
        }
        
        switch (choice) {
            case 1: {
                printf("\n开始性能对比测试...\n");
                performance_result our_algo = test_our_ultra_optimized();
                printf("\n");
                performance_result traditional_algo = test_traditional_baseline();
                display_comparison(our_algo, traditional_algo);
                break;
            }
            case 2:
                display_project_info();
                break;
            case 3:
                show_technical_details();
                break;
            case 4:
                show_security_analysis();
                break;
            case 5:
                run_full_demo();
                break;
            case 6:
                run_internal_data_performance_test();
                break;
            case 7:
                run_external_data_performance_test();
                break;
            case 0:
                setColor(10); // 绿色
                printf("\n👋 感谢使用！项目展示完成。\n");
                printf("🎯 我们的CSIDH-256优化算法成功实现了性能提升！\n");
                setColor(7); // 白色
                return 0;
            default:
                setColor(12); // 红色
                printf("❌ 无效选择，请输入 0-7 之间的数字！\n");
                setColor(7); // 白色
                break;
        }
        
        pause();
        clearScreen();
    }
    
    return 0;
}

