#include "external_test_data.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// 简单的JSON解析辅助函数
static void skip_whitespace(const char** str) {
    while (**str && isspace(**str)) {
        (*str)++;
    }
}

static bool parse_hex_string(const char* hex_str, uint64_t* value) {
    unsigned long long temp;
    int ret1 = sscanf(hex_str, "%llx", &temp);
    int ret2 = sscanf(hex_str, "0x%llx", &temp);
    if (ret1 == 1 || ret2 == 1) {
        *value = (uint64_t)temp;
        return true;
    }
    return false;
}

static bool parse_bigint256_from_json(const char* json_str, bigint256* result) {
    // 查找 "limbs" 字段
    const char* limbs_start = strstr(json_str, "\"limbs\"");
    if (!limbs_start) return false;
    
    limbs_start = strchr(limbs_start, '[');
    if (!limbs_start) return false;
    limbs_start++; // 跳过 '['
    
    // 解析4个limb值
    for (int i = 0; i < LIMBS; i++) {
        skip_whitespace(&limbs_start);
        
        // 跳过引号（如果有）
        if (*limbs_start == '"') limbs_start++;
        
        char hex_value[32] = {0};
        int j = 0;
        
        // 读取十六进制字符串（包括0x前缀）
        while (*limbs_start && *limbs_start != ',' && *limbs_start != ']' && *limbs_start != '"') {
            if (j < 31) hex_value[j++] = *limbs_start;
            limbs_start++;
        }
        hex_value[j] = '\0';
        
        // 跳过结束引号
        if (*limbs_start == '"') limbs_start++;
        
        // 解析十六进制字符串
        if (!parse_hex_string(hex_value, &result->limbs[i])) {
            return false;
        }
        
        // 跳过逗号和空白
        skip_whitespace(&limbs_start);
        if (*limbs_start == ',') {
            limbs_start++;
            skip_whitespace(&limbs_start);
        }
    }
    
    return true;
}

bool parse_test_vector_from_json(const char* json_str, external_test_vector* vector) {
    if (!json_str || !vector) return false;
    
    memset(vector, 0, sizeof(external_test_vector));
    
    // 解析test_name
    const char* name_start = strstr(json_str, "\"test_name\"");
    if (name_start) {
        name_start = strchr(name_start, ':');
        if (name_start) {
            name_start++;
            skip_whitespace(&name_start);
            if (*name_start == '"') name_start++;
            int i = 0;
            while (*name_start && *name_start != '"' && i < 127) {
                vector->test_name[i++] = *name_start++;
            }
            vector->test_name[i] = '\0';
        }
    }
    
    // 解析description
    const char* desc_start = strstr(json_str, "\"description\"");
    if (desc_start) {
        desc_start = strchr(desc_start, ':');
        if (desc_start) {
            desc_start++;
            skip_whitespace(&desc_start);
            if (*desc_start == '"') desc_start++;
            int i = 0;
            while (*desc_start && *desc_start != '"' && i < 255) {
                vector->description[i++] = *desc_start++;
            }
            vector->description[i] = '\0';
        }
    }
    
    // 解析category
    const char* cat_start = strstr(json_str, "\"category\"");
    if (cat_start) {
        cat_start = strchr(cat_start, ':');
        if (cat_start) {
            cat_start++;
            skip_whitespace(&cat_start);
            if (*cat_start == '"') cat_start++;
            int i = 0;
            while (*cat_start && *cat_start != '"' && i < 63) {
                vector->category[i++] = *cat_start++;
            }
            vector->category[i] = '\0';
        }
    }
    
    // 解析input_a
    const char* input_a_start = strstr(json_str, "\"input_a\"");
    if (input_a_start) {
        if (!parse_bigint256_from_json(input_a_start, &vector->input_a)) {
            return false;
        }
    } else {
        return false; // input_a是必需的
    }
    
    // 解析input_b
    const char* input_b_start = strstr(json_str, "\"input_b\"");
    if (input_b_start) {
        if (!parse_bigint256_from_json(input_b_start, &vector->input_b)) {
            return false;
        }
    } else {
        return false; // input_b是必需的
    }
    
    // 解析expected_result（可选）
    const char* expected_start = strstr(json_str, "\"expected_result\"");
    if (expected_start) {
        if (parse_bigint256_from_json(expected_start, &vector->expected_result)) {
            vector->has_expected_result = true;
        }
    }
    
    return true;
}

bool load_external_test_data(const char* filepath, external_test_dataset* dataset) {
    if (!filepath || !dataset) return false;
    
    FILE* file = fopen(filepath, "r");
    if (!file) {
        fprintf(stderr, "错误: 无法打开文件: %s\n", filepath);
        return false;
    }
    
    // 获取文件大小
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    if (file_size <= 0 || file_size > 10 * 1024 * 1024) { // 限制10MB
        fclose(file);
        fprintf(stderr, "错误: 文件大小无效或过大\n");
        return false;
    }
    
    // 读取文件内容
    char* buffer = (char*)malloc(file_size + 1);
    if (!buffer) {
        fclose(file);
        return false;
    }
    
    size_t read_size = fread(buffer, 1, file_size, file);
    fclose(file);
    buffer[read_size] = '\0';
    
    // 初始化数据集
    memset(dataset, 0, sizeof(external_test_dataset));
    strncpy(dataset->source_file, filepath, 255);
    dataset->source_file[255] = '\0';
    
    // 简单的JSON解析：查找测试向量数组
    const char* vectors_start = strstr(buffer, "\"test_vectors\"");
    if (!vectors_start) {
        vectors_start = strchr(buffer, '['); // 如果没有test_vectors字段，直接查找数组
    } else {
        vectors_start = strchr(vectors_start, '[');
    }
    
    if (!vectors_start) {
        free(buffer);
        fprintf(stderr, "错误: 未找到测试向量数组\n");
        return false;
    }
    
    // 计算向量数量（简单方法：数逗号）
    int max_vectors = 1000; // 最大向量数
    dataset->vectors = (external_test_vector*)calloc(max_vectors, sizeof(external_test_vector));
    if (!dataset->vectors) {
        free(buffer);
        return false;
    }
    
    // 解析每个测试向量
    const char* current = vectors_start + 1; // 跳过 '['
    int vector_count = 0;
    
    while (*current && vector_count < max_vectors) {
        skip_whitespace(&current);
        if (*current == ']') break;
        if (*current == '{') {
            // 使用括号匹配找到正确的对象结束位置
            const char* obj_start = current;
            int brace_count = 0;
            const char* obj_end = NULL;
            
            while (*current) {
                if (*current == '{') {
                    brace_count++;
                } else if (*current == '}') {
                    brace_count--;
                    if (brace_count == 0) {
                        obj_end = current;
                        break;
                    }
                }
                current++;
            }
            
            if (!obj_end) break;
            
            int obj_len = obj_end - obj_start + 1;
            char* obj_str = (char*)malloc(obj_len + 1);
            if (obj_str) {
                memcpy(obj_str, obj_start, obj_len);
                obj_str[obj_len] = '\0';
                
                if (parse_test_vector_from_json(obj_str, &dataset->vectors[vector_count])) {
                    vector_count++;
                } else {
                    // 调试输出：显示解析失败的JSON片段（前200字符）
                    char debug_str[201] = {0};
                    int debug_len = obj_len < 200 ? obj_len : 200;
                    memcpy(debug_str, obj_str, debug_len);
                    debug_str[debug_len] = '\0';
                    fprintf(stderr, "警告: 解析测试向量 %d 失败\n", vector_count + 1);
                    fprintf(stderr, "JSON片段: %s...\n", debug_str);
                }
                
                free(obj_str);
            }
            
            current = obj_end + 1;
            skip_whitespace(&current);
            if (*current == ',') current++;
        } else {
            current++;
        }
    }
    
    dataset->count = vector_count;
    dataset->loaded = (vector_count > 0);
    
    free(buffer);
    
    if (vector_count == 0) {
        free(dataset->vectors);
        dataset->vectors = NULL;
        fprintf(stderr, "错误: 未找到有效的测试向量\n");
        return false;
    }
    
    return true;
}

void free_external_test_dataset(external_test_dataset* dataset) {
    if (dataset) {
        if (dataset->vectors) {
            free(dataset->vectors);
            dataset->vectors = NULL;
        }
        dataset->count = 0;
        dataset->loaded = false;
    }
}

void print_external_test_dataset_info(const external_test_dataset* dataset) {
    if (!dataset || !dataset->loaded) {
        printf("数据集未加载\n");
        return;
    }
    
    printf("========================================\n");
    printf("外部测试数据集信息\n");
    printf("========================================\n");
    printf("源文件: %s\n", dataset->source_file);
    printf("测试向量数量: %d\n", dataset->count);
    printf("\n测试向量列表:\n");
    printf("----------------------------------------\n");
    
    for (int i = 0; i < dataset->count; i++) {
        const external_test_vector* vec = &dataset->vectors[i];
        printf("\n[%d] %s\n", i + 1, vec->test_name);
        printf("    类别: %s\n", vec->category[0] ? vec->category : "未指定");
        printf("    描述: %s\n", vec->description[0] ? vec->description : "无描述");
        printf("    输入A: 0x%016llX %016llX %016llX %016llX\n",
               (unsigned long long)vec->input_a.limbs[0],
               (unsigned long long)vec->input_a.limbs[1],
               (unsigned long long)vec->input_a.limbs[2],
               (unsigned long long)vec->input_a.limbs[3]);
        printf("    输入B: 0x%016llX %016llX %016llX %016llX\n",
               (unsigned long long)vec->input_b.limbs[0],
               (unsigned long long)vec->input_b.limbs[1],
               (unsigned long long)vec->input_b.limbs[2],
               (unsigned long long)vec->input_b.limbs[3]);
        if (vec->has_expected_result) {
            printf("    期望结果: 0x%016llX %016llX %016llX %016llX\n",
                   (unsigned long long)vec->expected_result.limbs[0],
                   (unsigned long long)vec->expected_result.limbs[1],
                   (unsigned long long)vec->expected_result.limbs[2],
                   (unsigned long long)vec->expected_result.limbs[3]);
        }
    }
    printf("========================================\n");
}

bool validate_external_test_data(const external_test_dataset* dataset) {
    if (!dataset || !dataset->loaded) {
        return false;
    }
    
    for (int i = 0; i < dataset->count; i++) {
        const external_test_vector* vec = &dataset->vectors[i];
        
        // 检查必需字段
        if (!vec->test_name[0]) {
            fprintf(stderr, "错误: 测试向量 %d 缺少test_name\n", i + 1);
            return false;
        }
        
        // 可以添加更多验证逻辑
    }
    
    return true;
}

