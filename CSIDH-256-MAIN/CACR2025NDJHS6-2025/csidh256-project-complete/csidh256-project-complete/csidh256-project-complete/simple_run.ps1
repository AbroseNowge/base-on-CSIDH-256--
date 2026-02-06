# 简单的密码学测试工具运行脚本

Write-Host "CSIDH-256 密码学测试工具运行脚本" -ForegroundColor Green
Write-Host ""

$msysPath = "F:\msys64\mingw64.exe"
$projectPath = "/c/Users/20513/Desktop/csidh256-project-complete/csidh256-project-complete/csidh256-project-complete"

# 检查MSYS2
if (Test-Path $msysPath) {
    Write-Host "找到MSYS2: $msysPath" -ForegroundColor Green
} else {
    Write-Host "未找到MSYS2: $msysPath" -ForegroundColor Red
    exit 1
}

Write-Host "开始编译密码学测试工具..." -ForegroundColor Yellow

# 编译测试向量生成器
Write-Host "编译测试向量生成器..." -ForegroundColor Cyan
$cmd1 = "$msysPath -c `"cd '$projectPath' && gcc -O3 -std=c99 -I../../src crypto_test_vectors_generator.c ../../src/mont_field.c ../../src/mont_field_optimized.c ../../src/traditional_mul.c -o crypto_test_vectors_generator.exe`""
Invoke-Expression $cmd1

# 编译增强版测试收集器
Write-Host "编译增强版测试收集器..." -ForegroundColor Cyan
$cmd2 = "$msysPath -c `"cd '$projectPath' && gcc -O3 -std=c99 -I../../src enhanced_crypto_test_collector.c ../../src/mont_field.c ../../src/mont_field_optimized.c ../../src/traditional_mul.c -o enhanced_crypto_test_collector.exe`""
Invoke-Expression $cmd2

# 编译性能测试数据生成器
Write-Host "编译性能测试数据生成器..." -ForegroundColor Cyan
$cmd3 = "$msysPath -c `"cd '$projectPath' && gcc -O3 -std=c99 -I../../src crypto_performance_test_data_generator.c ../../src/mont_field.c ../../src/mont_field_optimized.c ../../src/traditional_mul.c -o crypto_performance_test_data_generator.exe`""
Invoke-Expression $cmd3

# 编译测试数据验证器
Write-Host "编译测试数据验证器..." -ForegroundColor Cyan
$cmd4 = "$msysPath -c `"cd '$projectPath' && gcc -O3 -std=c99 -I../../src crypto_test_data_validator.c ../../src/mont_field.c ../../src/mont_field_optimized.c ../../src/traditional_mul.c -o crypto_test_data_validator.exe`""
Invoke-Expression $cmd4

Write-Host ""
Write-Host "开始运行测试工具..." -ForegroundColor Yellow

# 运行测试向量生成器
Write-Host "生成密码学测试向量..." -ForegroundColor Cyan
$cmd5 = "$msysPath -c `"cd '$projectPath' && echo '1' | ./crypto_test_vectors_generator.exe`""
Invoke-Expression $cmd5

# 运行增强版测试收集器
Write-Host "运行增强版性能测试..." -ForegroundColor Cyan
$cmd6 = "$msysPath -c `"cd '$projectPath' && ./enhanced_crypto_test_collector.exe`""
Invoke-Expression $cmd6

# 运行性能测试数据生成器
Write-Host "生成性能测试数据..." -ForegroundColor Cyan
$cmd7 = "$msysPath -c `"cd '$projectPath' && echo '1' | ./crypto_performance_test_data_generator.exe`""
Invoke-Expression $cmd7

# 运行测试数据验证器
Write-Host "验证测试数据..." -ForegroundColor Cyan
$cmd8 = "$msysPath -c `"cd '$projectPath' && echo '1' | ./crypto_test_data_validator.exe`""
Invoke-Expression $cmd8

Write-Host ""
Write-Host "测试完成！" -ForegroundColor Green
Write-Host "请检查生成的文件：" -ForegroundColor Yellow

# 查找生成的文件
$files = Get-ChildItem -Path . -Include "*.json", "*.csv", "*.txt", "*.log" -Recurse
foreach ($file in $files) {
    Write-Host "  $($file.Name)" -ForegroundColor White
}

Write-Host ""
Read-Host "按回车键退出"


