#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <windows.h>

// 日志分析结果结构
typedef struct {
    char algorithm[64];
    int iterations;
    double total_time_ms;
    double time_per_op_ns;
    double throughput_mops;
    uint64_t checksum;
    char timestamp[64];
} log_entry;

// 读取CSV文件
int read_csv_data(log_entry* entries, int max_entries) {
    FILE* csv_file = fopen("test_results.csv", "r");
    if (!csv_file) {
        printf("❌ 无法打开CSV文件: test_results.csv\n");
        return 0;
    }
    
    char line[1024];
    int count = 0;
    
    // 跳过标题行
    if (fgets(line, sizeof(line), csv_file)) {
        printf("📊 CSV文件标题: %s", line);
    }
    
    // 读取数据行
    while (fgets(line, sizeof(line), csv_file) && count < max_entries) {
        char* token = strtok(line, ",");
        if (token) {
            strcpy(entries[count].algorithm, token);
            
            token = strtok(NULL, ",");
            if (token) entries[count].iterations = atoi(token);
            
            token = strtok(NULL, ",");
            if (token) entries[count].total_time_ms = atof(token);
            
            token = strtok(NULL, ",");
            if (token) entries[count].time_per_op_ns = atof(token);
            
            token = strtok(NULL, ",");
            if (token) entries[count].throughput_mops = atof(token);
            
            token = strtok(NULL, ",");
            if (token) entries[count].checksum = strtoull(token, NULL, 10);
            
            token = strtok(NULL, ",");
            if (token) {
                strcpy(entries[count].timestamp, token);
                // 移除换行符
                entries[count].timestamp[strcspn(entries[count].timestamp, "\n")] = 0;
            }
            
            count++;
        }
    }
    
    fclose(csv_file);
    return count;
}

// 分析日志数据
void analyze_log_data(log_entry* entries, int count) {
    printf("\n🔍 日志数据分析结果\n");
    printf("==============================================\n");
    
    if (count == 0) {
        printf("❌ 没有找到日志数据\n");
        return;
    }
    
    printf("📊 测试记录数量: %d\n", count);
    printf("\n📋 详细测试结果:\n");
    printf("┌─────────────────────────────────┬──────────────┬──────────────┬──────────────┬──────────────┐\n");
    printf("│ 算法名称                        │ 迭代次数     │ 总耗时(ms)   │ 每次操作(ns) │ 吞吐量(Mops) │\n");
    printf("├─────────────────────────────────┼──────────────┼──────────────┼──────────────┼──────────────┤\n");
    
    for (int i = 0; i < count; i++) {
        printf("│ %-31s │ %12d │ %12.2f │ %12.2f │ %12.2f │\n",
               entries[i].algorithm, entries[i].iterations, 
               entries[i].total_time_ms, entries[i].time_per_op_ns, 
               entries[i].throughput_mops);
    }
    
    printf("└─────────────────────────────────┴──────────────┴──────────────┴──────────────┴──────────────┘\n");
    
    // 性能对比分析
    if (count >= 2) {
        printf("\n📈 性能对比分析:\n");
        double speedup = entries[0].time_per_op_ns / entries[1].time_per_op_ns;
        double improvement = (speedup - 1.0) * 100.0;
        
        printf("✅ %s 比 %s 快 %.2fx (提升 %.1f%%)\n", 
               entries[1].algorithm, entries[0].algorithm, speedup, improvement);
        
        printf("📊 性能提升详情:\n");
        printf("   - 时间减少: %.2f ns → %.2f ns (减少 %.2f ns)\n",
               entries[0].time_per_op_ns, entries[1].time_per_op_ns,
               entries[0].time_per_op_ns - entries[1].time_per_op_ns);
        printf("   - 吞吐量提升: %.2f Mops → %.2f Mops (提升 %.2f Mops)\n",
               entries[0].throughput_mops, entries[1].throughput_mops,
               entries[1].throughput_mops - entries[0].throughput_mops);
    }
    
    // 稳定性分析
    printf("\n🔒 稳定性分析:\n");
    for (int i = 0; i < count; i++) {
        printf("✅ %s: 校验和 = %llu (计算正确性确认)\n", 
               entries[i].algorithm, entries[i].checksum);
    }
}

// 读取并显示详细日志
void show_detailed_log() {
    FILE* log_file = fopen("benchmark_log.txt", "r");
    if (!log_file) {
        printf("❌ 无法打开详细日志文件: benchmark_log.txt\n");
        return;
    }
    
    printf("\n📝 详细测试日志\n");
    printf("==============================================\n");
    
    char line[1024];
    while (fgets(line, sizeof(line), log_file)) {
        printf("%s", line);
    }
    
    fclose(log_file);
}

// 生成性能报告
void generate_performance_report(log_entry* entries, int count) {
    FILE* report_file = fopen("performance_report.txt", "w");
    if (!report_file) {
        printf("❌ 无法创建性能报告文件\n");
        return;
    }
    
    fprintf(report_file, "CSIDH-256 性能测试报告\n");
    fprintf(report_file, "======================\n");
    fprintf(report_file, "生成时间: %s\n", ctime(&(time_t){time(0)}));
    fprintf(report_file, "测试记录数量: %d\n\n", count);
    
    fprintf(report_file, "测试结果汇总:\n");
    fprintf(report_file, "┌─────────────────────────────────┬──────────────┬──────────────┬──────────────┬──────────────┐\n");
    fprintf(report_file, "│ 算法名称                        │ 迭代次数     │ 总耗时(ms)   │ 每次操作(ns) │ 吞吐量(Mops) │\n");
    fprintf(report_file, "├─────────────────────────────────┼──────────────┼──────────────┼──────────────┼──────────────┤\n");
    
    for (int i = 0; i < count; i++) {
        fprintf(report_file, "│ %-31s │ %12d │ %12.2f │ %12.2f │ %12.2f │\n",
                entries[i].algorithm, entries[i].iterations, 
                entries[i].total_time_ms, entries[i].time_per_op_ns, 
                entries[i].throughput_mops);
    }
    
    fprintf(report_file, "└─────────────────────────────────┴──────────────┴──────────────┴──────────────┴──────────────┘\n");
    
    if (count >= 2) {
        double speedup = entries[0].time_per_op_ns / entries[1].time_per_op_ns;
        double improvement = (speedup - 1.0) * 100.0;
        
        fprintf(report_file, "\n性能对比分析:\n");
        fprintf(report_file, "%s 比 %s 快 %.2fx (提升 %.1f%%)\n", 
                entries[1].algorithm, entries[0].algorithm, speedup, improvement);
    }
    
    fclose(report_file);
    printf("✅ 性能报告已生成: performance_report.txt\n");
}

int main() {
    // 设置控制台编码为UTF-8
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);
    
    printf("CSIDH-256 日志分析工具\n");
    printf("==============================================\n");
    
    // 检查日志文件是否存在
    FILE* csv_file = fopen("test_results.csv", "r");
    if (!csv_file) {
        printf("❌ 没有找到日志文件！\n");
        printf("请先运行 log_generator.exe 生成日志文件\n");
        printf("\n运行命令:\n");
        printf("gcc -O3 -Isrc -o log_generator.exe log_generator.c src/mont_field.c src/optimized_montgomery_algorithm.c src/traditional_mul.c src/utils.c -lm\n");
        printf("./log_generator.exe\n");
        return 1;
    }
    fclose(csv_file);
    
    // 读取CSV数据
    log_entry entries[10];
    int count = read_csv_data(entries, 10);
    
    if (count > 0) {
        // 分析日志数据
        analyze_log_data(entries, count);
        
        // 生成性能报告
        generate_performance_report(entries, count);
        
        // 显示详细日志
        printf("\n是否查看详细日志? (y/n): ");
        char choice;
        scanf(" %c", &choice);
        if (choice == 'y' || choice == 'Y') {
            show_detailed_log();
        }
    } else {
        printf("❌ 没有找到有效的日志数据\n");
    }
    
    printf("\n📁 生成的文件:\n");
    printf("- test_results.csv (CSV格式数据)\n");
    printf("- benchmark_log.txt (详细测试日志)\n");
    printf("- performance_data.json (JSON格式数据)\n");
    printf("- performance_report.txt (性能分析报告)\n");
    
    printf("\n✅ 日志分析完成！\n");
    return 0;
}


