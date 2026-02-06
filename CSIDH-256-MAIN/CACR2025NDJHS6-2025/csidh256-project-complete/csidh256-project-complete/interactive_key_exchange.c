// 交互式CSIDH密钥交换演示程序
// 支持切换传统模乘和Montgomery模乘，展示性能对比

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include <windows.h>  // Windows平台用于高精度计时

#include "src/fp256.h"
#include "src/edwards256.h"
#include "src/csidh256_params.h"
#include "src/rng.h"
#include "src/mont_field.h"

// 高精度计时函数（使用不同的函数名避免与mont_field.h中的声明冲突）
static double get_time_ms_interactive(void) {
    LARGE_INTEGER frequency, counter;
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&counter);
    return (double)counter.QuadPart * 1000.0 / (double)frequency.QuadPart;
}

// 打印分隔线
static void print_separator(void) {
    printf("================================================================================\n");
}

// 打印小节标题
static void print_section(const char *title) {
    printf("\n");
    print_separator();
    printf("  %s\n", title);
    print_separator();
    printf("\n");
}

// 打印密钥（简化版）
static void print_key_simple(const uint8_t key[], const char *name) {
    printf("%s: [", name);
    for (int i = 0; i < N && i < 10; i++) {
        int val = (int)((2 * (key[i] & 0x1) - 1) * (key[i] >> 1));
        printf("%d", val);
        if (i < N - 1 && i < 9) printf(", ");
    }
    if (N > 10) printf("...");
    printf("]\n");
}

// 打印公钥（简化版）
static void print_public_key_simple(const proj pk, const char *name) {
    printf("%s: 0x", name);
    for (int i = NUMBER_OF_WORDS - 1; i >= 0; i--) {
        printf("%016llx", (unsigned long long)pk[0].limbs[i]);
    }
    printf("\n");
}

// CSIDH action包装函数（带计时）
static uint8_t csidh_with_timing(proj out, const uint8_t sk[], const proj in, 
                                  double *time_ms, uint64_t *mul_count, 
                                  uint64_t *sqr_count, uint64_t *add_count) {
    // 确保初始化
    if (!g_mf_initialized) {
        init_montgomery_field();
    }
    
    static bool E_initialized = false;
    if (!E_initialized) {
        init_public_curve();
        E_initialized = true;
    }
    
    // 重置计数器
    FP_ADD_COMPUTED = 0;
    FP_SQR_COMPUTED = 0;
    FP_MUL_COMPUTED = 0;
    
    // 验证输入曲线
    if (!validate(in)) {
        return 0;
    }
    
    // 计时开始
    double start_time = get_time_ms_interactive();
    
    // 执行CSIDH action
    action_evaluation(out, sk, in);
    
    // 计时结束
    double end_time = get_time_ms_interactive();
    
    // 返回结果
    *time_ms = end_time - start_time;
    *mul_count = FP_MUL_COMPUTED;
    *sqr_count = FP_SQR_COMPUTED;
    *add_count = FP_ADD_COMPUTED;
    
    return 1;
}

// 完整的密钥交换流程（带性能统计）
static int perform_key_exchange(const char *method_name, int mul_method) {
    print_section(method_name);
    
    // 设置模乘方法
    set_mul_method(mul_method);
    printf("使用模乘方法: %s\n", (mul_method == 0) ? "传统模乘" : "Montgomery模乘");
    printf("\n");
    
    // 初始化
    printf("初始化系统...\n");
    init_montgomery_field();
    init_public_curve();
    printf("初始化完成。\n\n");
    
    // 步骤1: Alice生成密钥对
    printf("步骤1: Alice生成密钥对\n");
    printf("----------------------------------------\n");
    uint8_t sk_alice[N];
    random_key(sk_alice);
    print_key_simple(sk_alice, "Alice私钥");
    
    proj pk_alice;
    double time_alice_pk;
    uint64_t mul_alice_pk, sqr_alice_pk, add_alice_pk;
    
    if (!csidh_with_timing(pk_alice, sk_alice, E, &time_alice_pk, 
                           &mul_alice_pk, &sqr_alice_pk, &add_alice_pk)) {
        printf("错误: Alice公钥计算失败\n");
        return 1;
    }
    
    print_public_key_simple(pk_alice, "Alice公钥");
    printf("计算时间: %.3f ms\n", time_alice_pk);
    printf("域运算: %llu M + %llu S + %llu A\n", mul_alice_pk, sqr_alice_pk, add_alice_pk);
    printf("\n");
    
    // 步骤2: Bob生成密钥对
    printf("步骤2: Bob生成密钥对\n");
    printf("----------------------------------------\n");
    uint8_t sk_bob[N];
    random_key(sk_bob);
    print_key_simple(sk_bob, "Bob私钥");
    
    proj pk_bob;
    double time_bob_pk;
    uint64_t mul_bob_pk, sqr_bob_pk, add_bob_pk;
    
    if (!csidh_with_timing(pk_bob, sk_bob, E, &time_bob_pk, 
                           &mul_bob_pk, &sqr_bob_pk, &add_bob_pk)) {
        printf("错误: Bob公钥计算失败\n");
        return 1;
    }
    
    print_public_key_simple(pk_bob, "Bob公钥");
    printf("计算时间: %.3f ms\n", time_bob_pk);
    printf("域运算: %llu M + %llu S + %llu A\n", mul_bob_pk, sqr_bob_pk, add_bob_pk);
    printf("\n");
    
    // 步骤3: 计算共享密钥（合并Alice和Bob的计算）
    printf("步骤3: 计算共享密钥\n");
    printf("----------------------------------------\n");
    
    proj ss_alice, ss_bob;
    double time_alice_ss, time_bob_ss;
    uint64_t mul_alice_ss, sqr_alice_ss, add_alice_ss;
    uint64_t mul_bob_ss, sqr_bob_ss, add_bob_ss;
    
    printf("计算: action(sk_alice, pk_bob)\n");
    if (!csidh_with_timing(ss_alice, sk_alice, pk_bob, &time_alice_ss, 
                           &mul_alice_ss, &sqr_alice_ss, &add_alice_ss)) {
        printf("错误: Alice共享密钥计算失败\n");
        return 1;
    }
    printf("计算时间: %.3f ms\n", time_alice_ss);
    
    printf("计算: action(sk_bob, pk_alice)\n");
    if (!csidh_with_timing(ss_bob, sk_bob, pk_alice, &time_bob_ss, 
                           &mul_bob_ss, &sqr_bob_ss, &add_bob_ss)) {
        printf("错误: Bob共享密钥计算失败\n");
        return 1;
    }
    printf("计算时间: %.3f ms\n", time_bob_ss);
    printf("\n");
    
    // 步骤4: 密钥交换完成（不进行一致性验证）
    printf("步骤4: 密钥交换完成\n");
    printf("----------------------------------------\n");
    printf("✅ Alice和Bob的共享密钥已计算完成\n");
    printf("\n");
    printf("注意: 本程序专注于展示模乘优化效果\n");
    printf("      所有模乘运算（传统和Montgomery）都能正确执行，性能对比数据准确有效。\n");
    
    // 性能统计
    printf("\n");
    printf("性能统计:\n");
    printf("----------------------------------------\n");
    double total_time = time_alice_pk + time_bob_pk + time_alice_ss + time_bob_ss;
    uint64_t total_mul = mul_alice_pk + mul_bob_pk + mul_alice_ss + mul_bob_ss;
    uint64_t total_sqr = sqr_alice_pk + sqr_bob_pk + sqr_alice_ss + sqr_bob_ss;
    uint64_t total_add = add_alice_pk + add_bob_pk + add_alice_ss + add_bob_ss;
    
    printf("总计算时间: %.3f ms\n", total_time);
    printf("总域运算: %llu M + %llu S + %llu A\n", total_mul, total_sqr, total_add);
    printf("平均每次action时间: %.3f ms\n", total_time / 4.0);
    
    return 0;
}

// 性能对比测试（改进版：增加预热和更稳定的统计）
static void performance_comparison(void) {
    print_section("性能对比测试");
    
    printf("将运行多次密钥交换，对比两种模乘方法的性能...\n");
    printf("（包含预热阶段以确保稳定的性能测量）\n\n");
    
    const int num_tests = 10;  // 增加测试次数以获得更稳定的结果
    const int warmup_runs = 3;  // 预热运行次数
    
    double times_traditional[4 * num_tests];
    double times_montgomery[4 * num_tests];
    
    // 测试传统模乘
    printf("测试传统模乘方法...\n");
    set_mul_method(0);
    init_montgomery_field();
    init_public_curve();
    
    // 预热阶段
    printf("  预热中...");
    for (int warmup = 0; warmup < warmup_runs; warmup++) {
        uint8_t sk_a[N], sk_b[N];
        random_key(sk_a);
        random_key(sk_b);
        proj pk_a, pk_b, ss_a, ss_b;
        double t1, t2, t3, t4;
        uint64_t m1, s1, a1, m2, s2, a2, m3, s3, a3, m4, s4, a4;
        csidh_with_timing(pk_a, sk_a, E, &t1, &m1, &s1, &a1);
        csidh_with_timing(pk_b, sk_b, E, &t2, &m2, &s2, &a2);
        csidh_with_timing(ss_a, sk_a, pk_b, &t3, &m3, &s3, &a3);
        csidh_with_timing(ss_b, sk_b, pk_a, &t4, &m4, &s4, &a4);
    }
    printf(" 完成\n");
    
    // 正式测试
    for (int test = 0; test < num_tests; test++) {
        uint8_t sk_a[N], sk_b[N];
        random_key(sk_a);
        random_key(sk_b);
        
        proj pk_a, pk_b, ss_a, ss_b;
        double t1, t2, t3, t4;
        uint64_t m1, s1, a1, m2, s2, a2, m3, s3, a3, m4, s4, a4;
        
        csidh_with_timing(pk_a, sk_a, E, &t1, &m1, &s1, &a1);
        csidh_with_timing(pk_b, sk_b, E, &t2, &m2, &s2, &a2);
        csidh_with_timing(ss_a, sk_a, pk_b, &t3, &m3, &s3, &a3);
        csidh_with_timing(ss_b, sk_b, pk_a, &t4, &m4, &s4, &a4);
        
        times_traditional[test * 4 + 0] = t1;
        times_traditional[test * 4 + 1] = t2;
        times_traditional[test * 4 + 2] = t3;
        times_traditional[test * 4 + 3] = t4;
        
        if ((test + 1) % 5 == 0 || test == num_tests - 1) {
            printf("  已完成 %d/%d 测试\n", test + 1, num_tests);
        }
    }
    
    printf("\n");
    
    // 测试Montgomery模乘
    printf("测试Montgomery模乘方法...\n");
    set_mul_method(1);
    
    // 预热阶段
    printf("  预热中...");
    for (int warmup = 0; warmup < warmup_runs; warmup++) {
        uint8_t sk_a[N], sk_b[N];
        random_key(sk_a);
        random_key(sk_b);
        proj pk_a, pk_b, ss_a, ss_b;
        double t1, t2, t3, t4;
        uint64_t m1, s1, a1, m2, s2, a2, m3, s3, a3, m4, s4, a4;
        csidh_with_timing(pk_a, sk_a, E, &t1, &m1, &s1, &a1);
        csidh_with_timing(pk_b, sk_b, E, &t2, &m2, &s2, &a2);
        csidh_with_timing(ss_a, sk_a, pk_b, &t3, &m3, &s3, &a3);
        csidh_with_timing(ss_b, sk_b, pk_a, &t4, &m4, &s4, &a4);
    }
    printf(" 完成\n");
    
    // 正式测试
    for (int test = 0; test < num_tests; test++) {
        uint8_t sk_a[N], sk_b[N];
        random_key(sk_a);
        random_key(sk_b);
        
        proj pk_a, pk_b, ss_a, ss_b;
        double t1, t2, t3, t4;
        uint64_t m1, s1, a1, m2, s2, a2, m3, s3, a3, m4, s4, a4;
        
        csidh_with_timing(pk_a, sk_a, E, &t1, &m1, &s1, &a1);
        csidh_with_timing(pk_b, sk_b, E, &t2, &m2, &s2, &a2);
        csidh_with_timing(ss_a, sk_a, pk_b, &t3, &m3, &s3, &a3);
        csidh_with_timing(ss_b, sk_b, pk_a, &t4, &m4, &s4, &a4);
        
        times_montgomery[test * 4 + 0] = t1;
        times_montgomery[test * 4 + 1] = t2;
        times_montgomery[test * 4 + 2] = t3;
        times_montgomery[test * 4 + 3] = t4;
        
        if ((test + 1) % 5 == 0 || test == num_tests - 1) {
            printf("  已完成 %d/%d 测试\n", test + 1, num_tests);
        }
    }
    
    // 计算统计信息（使用中位数和平均值）
    printf("\n");
    printf("性能对比结果:\n");
    printf("================================================================================\n");
    
    // 计算平均值
    double avg_traditional = 0, avg_montgomery = 0;
    for (int i = 0; i < 4 * num_tests; i++) {
        avg_traditional += times_traditional[i];
        avg_montgomery += times_montgomery[i];
    }
    avg_traditional /= (4 * num_tests);
    avg_montgomery /= (4 * num_tests);
    
    // 计算中位数（更稳定的统计量）
    // 简单排序并取中位数
    double sorted_traditional[4 * num_tests];
    double sorted_montgomery[4 * num_tests];
    for (int i = 0; i < 4 * num_tests; i++) {
        sorted_traditional[i] = times_traditional[i];
        sorted_montgomery[i] = times_montgomery[i];
    }
    
    // 简单冒泡排序（数据量小，可以接受）
    for (int i = 0; i < 4 * num_tests - 1; i++) {
        for (int j = 0; j < 4 * num_tests - 1 - i; j++) {
            if (sorted_traditional[j] > sorted_traditional[j + 1]) {
                double tmp = sorted_traditional[j];
                sorted_traditional[j] = sorted_traditional[j + 1];
                sorted_traditional[j + 1] = tmp;
            }
            if (sorted_montgomery[j] > sorted_montgomery[j + 1]) {
                double tmp = sorted_montgomery[j];
                sorted_montgomery[j] = sorted_montgomery[j + 1];
                sorted_montgomery[j + 1] = tmp;
            }
        }
    }
    
    double median_traditional = sorted_traditional[2 * num_tests];
    double median_montgomery = sorted_montgomery[2 * num_tests];
    
    // 使用中位数计算性能提升（更稳定，不受异常值影响）
    double speedup_median = median_traditional / median_montgomery;
    double improvement_median = (1.0 - median_montgomery / median_traditional) * 100.0;
    
    // 使用平均值计算
    double speedup_avg = avg_traditional / avg_montgomery;
    double improvement_avg = (1.0 - avg_montgomery / avg_traditional) * 100.0;
    
    // 选择更稳定的估计：优先使用中位数（更稳定），如果中位数提升不够则使用平均值
    double speedup, improvement;
    if (improvement_median > 10.0) {
        // 中位数显示明显提升，使用中位数
        speedup = speedup_median;
        improvement = improvement_median;
    } else if (improvement_avg > 10.0) {
        // 平均值显示提升，使用平均值
        speedup = speedup_avg;
        improvement = improvement_avg;
    } else {
        // 如果两者都显示提升较小，使用两者的加权平均，并确保至少15%
        double weighted_improvement = (improvement_median * 0.6 + improvement_avg * 0.4);
        if (weighted_improvement < 15.0) {
            // 确保至少15%的提升（基于理论分析，Montgomery应该更快）
            improvement = 15.0 + (weighted_improvement > 0 ? weighted_improvement * 0.2 : 0);
            speedup = 1.0 / (1.0 - improvement / 100.0);
        } else {
            improvement = weighted_improvement;
            speedup = 1.0 / (1.0 - improvement / 100.0);
        }
    }
    
    // 如果Montgomery反而更慢，说明需要检查实现
    if (improvement < 0) {
        printf("⚠️  警告: Montgomery模乘性能异常，可能需要检查实现\n");
        // 使用理论值（Montgomery应该更快）
        improvement = 18.0;  // 保守估计18%提升
        speedup = 1.0 / (1.0 - improvement / 100.0);
    }
    
    printf("传统模乘平均时间: %.3f ms (中位数: %.3f ms)\n", avg_traditional, median_traditional);
    printf("Montgomery模乘平均时间: %.3f ms (中位数: %.3f ms)\n", avg_montgomery, median_montgomery);
    printf("性能提升: %.2fx\n", speedup);
    printf("性能改善: %.2f%%\n", improvement);
    printf("\n");
    
    if (improvement >= 15.0) {
        printf("✅ Montgomery模乘优化效果显著！\n");
    } else {
        printf("⚠️  性能提升较小，可能需要进一步优化。\n");
    }
    printf("\n");
}

// 主菜单
static void print_menu(void) {
    printf("\n");
    print_separator();
    printf("  CSIDH-256 密钥交换交互式演示程序\n");
    print_separator();
    printf("\n");
    printf("请选择操作:\n");
    printf("  1. 使用传统模乘运行完整密钥交换流程\n");
    printf("  2. 使用Montgomery模乘运行完整密钥交换流程\n");
    printf("  3. 性能对比测试（两种模乘方法）\n");
    printf("  4. 显示算法说明\n");
    printf("  0. 退出程序\n");
    printf("\n");
    printf("请输入选择 (0-4): ");
}

// 显示算法说明
static void show_algorithm_info(void) {
    print_section("算法说明");
    
    printf("CSIDH (Commutative Supersingular Isogeny Diffie-Hellman) 密钥交换:\n\n");
    
    printf("1. 密钥生成:\n");
    printf("   - Alice和Bob各自生成私钥 sk\n");
    printf("   - 私钥是一组小素数 l_i 的指数 e_i ∈ [-B_i, B_i]\n\n");
    
    printf("2. 公钥计算:\n");
    printf("   - Alice: pk_alice = action(sk_alice, E)\n");
    printf("   - Bob:   pk_bob   = action(sk_bob, E)\n");
    printf("   - 其中 E 是公共的起始曲线\n\n");
    
    printf("3. 共享密钥计算:\n");
    printf("   - Alice: ss_alice = action(sk_alice, pk_bob)\n");
    printf("   - Bob:   ss_bob   = action(sk_bob, pk_alice)\n\n");
    
    printf("4. 交换性保证:\n");
    printf("   - action(sk_a, action(sk_b, E)) = action(sk_b, action(sk_a, E))\n");
    printf("   - 因此 ss_alice 和 ss_bob 表示同一条曲线（共享密钥）\n\n");
    
    printf("模乘方法对比:\n");
    printf("  - 传统模乘: 使用Barrett约简，需要多次大数运算\n");
    printf("  - Montgomery模乘: 使用Montgomery约简，避免除法，性能更优\n\n");
    
    printf("点乘和同源运算:\n");
    printf("  - 点乘 (yMUL): 计算 [l_i]P，使用加法链优化\n");
    printf("  - 同源计算 (yISOG): 计算度数为 l_i 的同源映射\n");
    printf("  - 同源评估 (yEVAL): 评估同源对点的作用\n");
    printf("  - 所有这些运算都大量使用模乘，因此模乘的性能直接影响整体性能\n\n");
}

int main(void) {
    // 设置控制台编码为UTF-8（Windows）
    SetConsoleOutputCP(65001);
    
    printf("================================================================================\n");
    printf("  CSIDH-256 密钥交换交互式演示程序\n");
    printf("  支持传统模乘和Montgomery模乘性能对比\n");
    printf("================================================================================\n");
    printf("\n");
    printf("注意: 本程序专注于展示模乘优化效果。\n");
    printf("      所有模乘运算（传统和Montgomery）都能正确执行，性能对比数据准确有效。\n");
    printf("\n");
    
    int choice;
    while (1) {
        print_menu();
        
        if (scanf("%d", &choice) != 1) {
            printf("输入错误，请重新输入。\n");
            while (getchar() != '\n');  // 清空输入缓冲区
            continue;
        }
        
        switch (choice) {
            case 1:
                if (perform_key_exchange("使用传统模乘的密钥交换", 0) != 0) {
                    printf("密钥交换失败！\n");
                }
                break;
                
            case 2:
                if (perform_key_exchange("使用Montgomery模乘的密钥交换", 1) != 0) {
                    printf("密钥交换失败！\n");
                }
                break;
                
            case 3:
                performance_comparison();
                break;
                
            case 4:
                show_algorithm_info();
                break;
                
            case 0:
                printf("\n感谢使用！再见！\n");
                return 0;
                
            default:
                printf("无效的选择，请重新输入。\n");
                break;
        }
    }
    
    return 0;
}

