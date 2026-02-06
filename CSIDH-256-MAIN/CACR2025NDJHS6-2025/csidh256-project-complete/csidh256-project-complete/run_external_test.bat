@echo off
chcp 65001 >nul
setlocal enabledelayedexpansion

echo ================================================
echo CSIDH-256 外部测试数据性能测试
echo ================================================
echo.

REM 检查是否已编译
if not exist "performance_test_with_external.exe" (
    echo [错误] 程序未编译，正在编译...
    echo.
    
    REM 尝试使用Makefile编译
    if exist "Makefile" (
        make performance_test_with_external.exe
        if errorlevel 1 (
            echo [错误] 编译失败，请检查编译环境
            pause
            exit /b 1
        )
    ) else (
        echo [错误] 未找到Makefile，请手动编译
        echo 编译命令: gcc -O3 -Wall -Isrc -o performance_test_with_external.exe ^
        echo   performance_test_with_external.c ^
        echo   src/external_test_data.c ^
        echo   src/mont_field.c ^
        echo   src/optimized_montgomery_algorithm.c ^
        echo   src/traditional_mul.c ^
        echo   src/utils.c -lm
        pause
        exit /b 1
    )
    echo.
)

REM 检查外部测试数据文件
if not exist "test_data_external.json" (
    echo [警告] 未找到 test_data_external.json
    echo 将使用内置测试数据
    echo.
    performance_test_with_external.exe
) else (
    echo 检测到外部测试数据文件: test_data_external.json
    echo.
    
    REM 显示菜单
    echo 请选择测试模式:
    echo   1. 使用内置测试数据（默认）
    echo   2. 使用外部测试数据（第一个向量）
    echo   3. 列出外部测试数据中的所有向量
    echo   4. 使用外部测试数据的指定向量
    echo   5. 使用外部测试数据（自定义迭代次数）
    echo   0. 退出
    echo.
    set /p choice="请输入选项 (0-5): "
    
    if "!choice!"=="1" (
        echo.
        echo [模式1] 使用内置测试数据
        echo.
        performance_test_with_external.exe
    ) else if "!choice!"=="2" (
        echo.
        echo [模式2] 使用外部测试数据（第一个向量）
        echo.
        performance_test_with_external.exe --external test_data_external.json
    ) else if "!choice!"=="3" (
        echo.
        echo [模式3] 列出所有测试向量
        echo.
        performance_test_with_external.exe --external test_data_external.json --list
    ) else if "!choice!"=="4" (
        echo.
        set /p vector_index="请输入向量索引 (从1开始): "
        echo [模式4] 使用外部测试数据的第 !vector_index! 个向量
        echo.
        performance_test_with_external.exe --external test_data_external.json --vector !vector_index!
    ) else if "!choice!"=="5" (
        echo.
        set /p iterations="请输入迭代次数 (默认100000): "
        if "!iterations!"=="" set iterations=100000
        echo [模式5] 使用外部测试数据，迭代次数: !iterations!
        echo.
        performance_test_with_external.exe --external test_data_external.json --iterations !iterations!
    ) else if "!choice!"=="0" (
        echo 退出
        exit /b 0
    ) else (
        echo 无效选项
        pause
        exit /b 1
    )
)

echo.
echo ================================================
echo 测试完成
echo ================================================
pause


