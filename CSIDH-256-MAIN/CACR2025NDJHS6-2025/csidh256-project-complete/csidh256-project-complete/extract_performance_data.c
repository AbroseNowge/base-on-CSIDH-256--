#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void extract_performance_metrics(const char* filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("无法打开文件: %s\n", filename);
        return;
    }
    
    char line[256];
    printf("分析文件: %s\n", filename);
    printf("----------------------------------------\n");
    
    while (fgets(line, sizeof(line), file)) {
        // 提取关键性能信息
        if (strstr(line, "time") || strstr(line, "Time") || 
            strstr(line, "performance") || strstr(line, "Performance") ||
            strstr(line, "cycles") || strstr(line, "Cycles")) {
            printf("性能指标: %s", line);
        }
        
        if (strstr(line, "optimization") || strstr(line, "Optimization")) {
            printf("优化信息: %s", line);
        }
        
        if (strstr(line, "improvement") || strstr(line, "Improvement") ||
            strstr(line, "better") || strstr(line, "faster")) {
            printf("改进数据: %s", line);
        }
    }
    
    fclose(file);
    printf("\n");
}

int main() {
    printf("=== 测试数据提取报告 ===\n\n");
    
    // 分析各个性能测试文件
    extract_performance_metrics("performance_basic.txt");
    extract_performance_metrics("performance_comparison.txt"); 
    extract_performance_metrics("performance_extreme.txt");
    extract_performance_metrics("performance_standard.txt");
    extract_performance_metrics("performance_ultra.txt");
    
    printf("=== 数据提取完成 ===\n");
    return 0;
}
