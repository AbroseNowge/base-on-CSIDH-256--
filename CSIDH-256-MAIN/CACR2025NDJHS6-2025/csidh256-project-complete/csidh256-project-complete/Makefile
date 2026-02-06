# CSIDH-256 后量子密码算法优化项目 Makefile
CC = gcc
CFLAGS = -O3 -Wall -Wno-unused-const-variable -march=native -mtune=native -fopenmp -Isrc
LIBS = -lm

# 源文件
TRADITIONAL_ALGORITHM_SRC = src/traditional_mul.c
OPTIMIZED_ALGORITHM_SRC = src/optimized_montgomery_algorithm.c
BASIC_MONTGOMERY_SRC = src/mont_field.c
UTILS_SRC = src/utils.c

# 测试程序源文件
PERFORMANCE_TEST_SRC = performance_comparison_test.c
PERFORMANCE_TEST_EXTERNAL_SRC = performance_test_with_external.c
INTERACTIVE_DEMO_SRC = interactive_demo_program.c
DATA_COLLECTOR_SRC = test_data_collector.c
EXTERNAL_DATA_SRC = src/external_test_data.c

# 目标文件
PERFORMANCE_TEST_TARGET = performance_comparison_test.exe
PERFORMANCE_TEST_EXTERNAL_TARGET = performance_test_with_external.exe
INTERACTIVE_DEMO_TARGET = interactive_demo_program.exe
DATA_COLLECTOR_TARGET = test_data_collector.exe

# 默认目标
all: $(PERFORMANCE_TEST_TARGET) $(PERFORMANCE_TEST_EXTERNAL_TARGET) $(INTERACTIVE_DEMO_TARGET) $(DATA_COLLECTOR_TARGET)

# 编译性能对比测试
$(PERFORMANCE_TEST_TARGET): $(PERFORMANCE_TEST_SRC) $(BASIC_MONTGOMERY_SRC) $(OPTIMIZED_ALGORITHM_SRC) $(TRADITIONAL_ALGORITHM_SRC) $(UTILS_SRC)
	$(CC) $(CFLAGS) -o $(PERFORMANCE_TEST_TARGET) $(PERFORMANCE_TEST_SRC) $(BASIC_MONTGOMERY_SRC) $(OPTIMIZED_ALGORITHM_SRC) $(TRADITIONAL_ALGORITHM_SRC) $(UTILS_SRC) $(LIBS)

# 编译交互式演示程序
$(INTERACTIVE_DEMO_TARGET): $(INTERACTIVE_DEMO_SRC) $(BASIC_MONTGOMERY_SRC) $(OPTIMIZED_ALGORITHM_SRC) $(TRADITIONAL_ALGORITHM_SRC) $(UTILS_SRC)
	$(CC) $(CFLAGS) -o $(INTERACTIVE_DEMO_TARGET) $(INTERACTIVE_DEMO_SRC) $(BASIC_MONTGOMERY_SRC) $(OPTIMIZED_ALGORITHM_SRC) $(TRADITIONAL_ALGORITHM_SRC) $(UTILS_SRC) $(LIBS)

# 编译支持外部数据的性能测试
$(PERFORMANCE_TEST_EXTERNAL_TARGET): $(PERFORMANCE_TEST_EXTERNAL_SRC) $(EXTERNAL_DATA_SRC) $(BASIC_MONTGOMERY_SRC) $(OPTIMIZED_ALGORITHM_SRC) $(TRADITIONAL_ALGORITHM_SRC) $(UTILS_SRC)
	$(CC) $(CFLAGS) -o $(PERFORMANCE_TEST_EXTERNAL_TARGET) $(PERFORMANCE_TEST_EXTERNAL_SRC) $(EXTERNAL_DATA_SRC) $(BASIC_MONTGOMERY_SRC) $(OPTIMIZED_ALGORITHM_SRC) $(TRADITIONAL_ALGORITHM_SRC) $(UTILS_SRC) $(LIBS)

# 编译测试数据收集程序
$(DATA_COLLECTOR_TARGET): $(DATA_COLLECTOR_SRC) $(BASIC_MONTGOMERY_SRC) $(OPTIMIZED_ALGORITHM_SRC) $(TRADITIONAL_ALGORITHM_SRC) $(UTILS_SRC)
	$(CC) $(CFLAGS) -o $(DATA_COLLECTOR_TARGET) $(DATA_COLLECTOR_SRC) $(BASIC_MONTGOMERY_SRC) $(OPTIMIZED_ALGORITHM_SRC) $(TRADITIONAL_ALGORITHM_SRC) $(UTILS_SRC) $(LIBS)

# 运行性能测试
run-performance: $(PERFORMANCE_TEST_TARGET)
	./$(PERFORMANCE_TEST_TARGET)

# 运行支持外部数据的性能测试
run-performance-external: $(PERFORMANCE_TEST_EXTERNAL_TARGET)
	./$(PERFORMANCE_TEST_EXTERNAL_TARGET)

# 运行交互式演示
run-demo: $(INTERACTIVE_DEMO_TARGET)
	./$(INTERACTIVE_DEMO_TARGET)

# 运行数据收集
run-data-collector: $(DATA_COLLECTOR_TARGET)
	./$(DATA_COLLECTOR_TARGET)

# 清理
clean:
	rm -f $(PERFORMANCE_TEST_TARGET) $(PERFORMANCE_TEST_EXTERNAL_TARGET) $(INTERACTIVE_DEMO_TARGET) $(DATA_COLLECTOR_TARGET)

# 帮助
help:
	@echo "可用命令："
	@echo "  make                         - 编译所有程序"
	@echo "  make run-performance         - 编译并运行性能测试"
	@echo "  make run-performance-external - 编译并运行支持外部数据的性能测试"
	@echo "  make run-demo                - 编译并运行交互式演示"
	@echo "  make run-data-collector      - 编译并运行数据收集"
	@echo "  make clean                   - 清理编译文件"
	@echo "  make help                    - 显示帮助信息"

.PHONY: all run-performance run-demo run-data-collector clean help