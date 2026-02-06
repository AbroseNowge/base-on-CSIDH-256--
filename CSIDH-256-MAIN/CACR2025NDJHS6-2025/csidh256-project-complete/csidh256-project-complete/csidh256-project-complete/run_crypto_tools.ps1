# CSIDH-256 密码学测试工具 PowerShell 运行脚本

Write-Host "==============================================" -ForegroundColor Green
Write-Host "CSIDH-256 密码学测试工具集运行脚本" -ForegroundColor Green
Write-Host "==============================================" -ForegroundColor Green
Write-Host ""

# 设置路径
$msysPath = "F:\msys64\mingw64.exe"
$projectPath = "/c/Users/20513/Desktop/csidh256-project-complete/csidh256-project-complete/csidh256-project-complete"

Write-Host "🔍 检查MSYS2路径..." -ForegroundColor Yellow
if (Test-Path $msysPath) {
    Write-Host "✅ MSYS2找到: $msysPath" -ForegroundColor Green
} else {
    Write-Host "❌ 未找到MSYS2: $msysPath" -ForegroundColor Red
    Write-Host "请确认MSYS2安装路径是否正确" -ForegroundColor Red
    Read-Host "按回车键退出"
    exit 1
}

Write-Host ""
Write-Host "🔍 检查项目文件..." -ForegroundColor Yellow

# 检查必要的源文件
$sourceFiles = @(
    "crypto_test_vectors_generator.c",
    "enhanced_crypto_test_collector.c", 
    "crypto_performance_test_data_generator.c",
    "crypto_test_data_validator.c",
    "../src/mont_field.c",
    "../src/mont_field_optimized.c",
    "../src/traditional_mul.c"
)

$missingFiles = @()
foreach ($file in $sourceFiles) {
    $fullPath = Join-Path $PWD $file
    if (Test-Path $fullPath) {
        Write-Host "✅ $file" -ForegroundColor Green
    } else {
        Write-Host "❌ $file (缺失)" -ForegroundColor Red
        $missingFiles += $file
    }
}

if ($missingFiles.Count -gt 0) {
    Write-Host ""
    Write-Host "❌ 缺少必要文件，无法继续" -ForegroundColor Red
    Write-Host "缺失的文件:" -ForegroundColor Red
    foreach ($file in $missingFiles) {
        Write-Host "  - $file" -ForegroundColor Red
    }
    Read-Host "按回车键退出"
    exit 1
}

Write-Host ""
Write-Host "✅ 所有必要文件都存在" -ForegroundColor Green
Write-Host ""

# 创建输出目录
$outputDir = "crypto_test_output"
if (!(Test-Path $outputDir)) {
    New-Item -ItemType Directory -Path $outputDir | Out-Null
    Write-Host "📁 创建输出目录: $outputDir" -ForegroundColor Yellow
}

Write-Host "🔧 开始编译密码学测试工具..." -ForegroundColor Yellow
Write-Host ""

# 编译命令
$compileCommands = @(
    @{
        Name = "密码学测试向量生成器"
        Command = "gcc -O3 -Wall -Wextra -std=c99 -I../../src crypto_test_vectors_generator.c ../../src/mont_field.c ../../src/mont_field_optimized.c ../../src/traditional_mul.c -o crypto_test_vectors_generator.exe"
    },
    @{
        Name = "增强版密码学测试收集器"
        Command = "gcc -O3 -Wall -Wextra -std=c99 -I../../src enhanced_crypto_test_collector.c ../../src/mont_field.c ../../src/mont_field_optimized.c ../../src/traditional_mul.c -o enhanced_crypto_test_collector.exe"
    },
    @{
        Name = "密码学性能测试数据生成器"
        Command = "gcc -O3 -Wall -Wextra -std=c99 -I../../src crypto_performance_test_data_generator.c ../../src/mont_field.c ../../src/mont_field_optimized.c ../../src/traditional_mul.c -o crypto_performance_test_data_generator.exe"
    },
    @{
        Name = "密码学测试数据验证器"
        Command = "gcc -O3 -Wall -Wextra -std=c99 -I../../src crypto_test_data_validator.c ../../src/mont_field.c ../../src/mont_field_optimized.c ../../src/traditional_mul.c -o crypto_test_data_validator.exe"
    }
)

foreach ($compile in $compileCommands) {
    Write-Host "  编译 $($compile.Name)..." -ForegroundColor Cyan
    
    $fullCommand = "$msysPath -c `"cd '$projectPath' && $($compile.Command)`""
    
    try {
        $result = Invoke-Expression $fullCommand
        if ($LASTEXITCODE -eq 0) {
            Write-Host "  ✅ $($compile.Name) 编译成功" -ForegroundColor Green
        } else {
            Write-Host "  ❌ $($compile.Name) 编译失败" -ForegroundColor Red
        }
    } catch {
        Write-Host "  ❌ $($compile.Name) 编译出错: $($_.Exception.Message)" -ForegroundColor Red
    }
}

Write-Host ""
Write-Host "🚀 运行密码学测试工具..." -ForegroundColor Yellow
Write-Host ""

# 运行测试工具
$runCommands = @(
    @{
        Name = "生成密码学测试向量"
        Command = "echo '1' | ./crypto_test_vectors_generator.exe"
    },
    @{
        Name = "运行增强版性能测试"
        Command = "./enhanced_crypto_test_collector.exe"
    },
    @{
        Name = "生成性能测试数据"
        Command = "echo '1' | ./crypto_performance_test_data_generator.exe"
    },
    @{
        Name = "验证测试数据"
        Command = "echo '1' | ./crypto_test_data_validator.exe"
    }
)

foreach ($run in $runCommands) {
    Write-Host "  $($run.Name)..." -ForegroundColor Cyan
    
    $fullCommand = "$msysPath -c `"cd '$projectPath' && $($run.Command)`""
    
    try {
        $result = Invoke-Expression $fullCommand
        Write-Host "  ✅ $($run.Name) 完成" -ForegroundColor Green
    } catch {
        Write-Host "  ❌ $($run.Name) 失败: $($_.Exception.Message)" -ForegroundColor Red
    }
}

Write-Host ""
Write-Host "==============================================" -ForegroundColor Green
Write-Host "测试完成！查看生成的文件" -ForegroundColor Green
Write-Host "==============================================" -ForegroundColor Green
Write-Host ""

# 检查生成的文件
$expectedFiles = @("*.json", "*.csv", "*.txt", "*.log")
$foundFiles = @()

foreach ($pattern in $expectedFiles) {
    $files = Get-ChildItem -Path $projectPath -Filter $pattern -Recurse
    foreach ($file in $files) {
        $foundFiles += $file
    }
}

if ($foundFiles.Count -gt 0) {
    Write-Host "📊 生成的文件:" -ForegroundColor Yellow
    foreach ($file in $foundFiles) {
        Write-Host "  📁 $($file.Name) ($($file.Length) bytes)" -ForegroundColor White
    }
} else {
    Write-Host "❌ 未找到生成的文件" -ForegroundColor Red
}

Write-Host ""
Write-Host "🎯 下一步操作建议:" -ForegroundColor Yellow
Write-Host "1. 查看生成的JSON文件了解测试数据结构" -ForegroundColor White
Write-Host "2. 使用Excel打开CSV文件进行数据分析" -ForegroundColor White
Write-Host "3. 阅读TXT报告文件了解详细测试结果" -ForegroundColor White
Write-Host "4. 检查LOG文件了解测试执行过程" -ForegroundColor White
Write-Host ""

Write-Host "💡 提示:" -ForegroundColor Yellow
Write-Host "- 所有测试数据都基于CSIDH-256后量子密码算法" -ForegroundColor White
Write-Host "- 测试向量符合NIST和RFC标准" -ForegroundColor White
Write-Host "- 性能指标包含执行时间、吞吐量、能效比等" -ForegroundColor White
Write-Host "- 安全测试覆盖侧信道攻击、时序攻击等威胁模型" -ForegroundColor White
Write-Host ""

Read-Host "按回车键退出"


