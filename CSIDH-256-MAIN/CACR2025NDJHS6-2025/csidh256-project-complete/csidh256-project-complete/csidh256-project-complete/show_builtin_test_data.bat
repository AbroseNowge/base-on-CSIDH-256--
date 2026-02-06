@echo off
chcp 65001 >nul
echo ==============================================
echo CSIDH-256 内置测试数据详细展示
echo ==============================================
echo.

set MSYS_PATH=F:\msys64\mingw64.exe
set PROJECT_PATH=/c/Users/20513/Desktop/csidh256-project-complete/csidh256-project-complete/csidh256-project-complete

echo 🔍 查看内置测试数据结构...
echo.

echo 📊 1. 测试数据收集器中的内置测试向量:
echo ----------------------------------------------
"%MSYS_PATH%" -c "cd '%PROJECT_PATH%' && grep -A 10 'TEST_A.*=' test_data_collector.c || echo '查找测试向量...'"

echo.
echo 📊 2. 测试数据生成器中的内置测试数据:
echo ----------------------------------------------
"%MSYS_PATH%" -c "cd '%PROJECT_PATH%' && grep -A 5 'generate.*test' test_data_generator.c || echo '查找测试生成函数...'"

echo.
echo 📊 3. 性能测试程序中的内置基准数据:
echo ----------------------------------------------
"%MSYS_PATH%" -c "cd '%PROJECT_PATH%' && grep -A 5 'iterations.*=' performance_test.c || echo '查找性能测试参数...'"

echo.
echo 📊 4. 运行内置测试并显示详细结果:
echo ----------------------------------------------
echo 正在运行内置测试数据收集器...
"%MSYS_PATH%" -c "cd '%PROJECT_PATH%' && ./test_data_collector.exe" 2>&1

echo.
echo 正在运行内置测试数据生成器...
"%MSYS_PATH%" -c "cd '%PROJECT_PATH%' && ./test_data_generator.exe" 2>&1

echo.
echo 正在运行内置性能测试...
"%MSYS_PATH%" -c "cd '%PROJECT_PATH%' && ./performance_test.exe" 2>&1

echo.
echo 📊 5. 查看生成的内置测试数据文件:
echo ----------------------------------------------
"%MSYS_PATH%" -c "cd '%PROJECT_PATH%' && echo '查找所有测试数据文件:' && ls -la *.txt *.csv *.data *.log 2>/dev/null || echo '未找到测试数据文件'"

echo.
echo 📊 6. 内置测试数据内容分析:
echo ----------------------------------------------
echo 正在分析内置测试数据内容...
"%MSYS_PATH%" -c "cd '%PROJECT_PATH%' && for file in *.txt *.csv *.data *.log; do if [ -f \"\$file\" ]; then echo \"=== 文件: \$file ===\"; head -20 \"\$file\"; echo; fi; done"

echo.
echo ==============================================
echo 内置测试数据详细展示完成
echo ==============================================
echo.

echo 🎯 内置测试数据总结:
echo - 包含标准化的测试向量
echo - 提供性能基准测试数据
echo - 支持算法正确性验证
echo - 生成详细的测试报告
echo.

pause


