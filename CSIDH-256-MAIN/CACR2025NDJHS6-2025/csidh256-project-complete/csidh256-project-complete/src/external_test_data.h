#ifndef EXTERNAL_TEST_DATA_H
#define EXTERNAL_TEST_DATA_H

#include "params.h"
#include <stdbool.h>

// 外部测试数据向量结构
typedef struct {
    char test_name[128];        // 测试名称
    char description[256];      // 测试描述
    char category[64];          // 测试类别（STANDARD, EDGE, SECURITY, PERFORMANCE等）
    bigint256 input_a;          // 输入A
    bigint256 input_b;          // 输入B
    bigint256 expected_result;  // 期望结果（可选）
    bool has_expected_result;   // 是否有期望结果
} external_test_vector;

// 外部测试数据集结构
typedef struct {
    external_test_vector* vectors;  // 测试向量数组
    int count;                      // 向量数量
    char source_file[256];          // 源文件路径
    bool loaded;                    // 是否已加载
} external_test_dataset;

/**
 * 从JSON文件加载外部测试数据
 * @param filepath JSON文件路径
 * @param dataset 输出的测试数据集
 * @return 成功返回true，失败返回false
 */
bool load_external_test_data(const char* filepath, external_test_dataset* dataset);

/**
 * 释放外部测试数据集内存
 * @param dataset 要释放的数据集
 */
void free_external_test_dataset(external_test_dataset* dataset);

/**
 * 打印外部测试数据集信息
 * @param dataset 测试数据集
 */
void print_external_test_dataset_info(const external_test_dataset* dataset);

/**
 * 从JSON字符串解析单个测试向量
 * @param json_str JSON字符串
 * @param vector 输出的测试向量
 * @return 成功返回true，失败返回false
 */
bool parse_test_vector_from_json(const char* json_str, external_test_vector* vector);

/**
 * 验证外部测试数据格式
 * @param dataset 测试数据集
 * @return 有效返回true，无效返回false
 */
bool validate_external_test_data(const external_test_dataset* dataset);

#endif // EXTERNAL_TEST_DATA_H


